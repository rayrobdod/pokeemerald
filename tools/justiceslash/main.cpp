#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <iostream>
#include "AlgebraicFunc.h"
#include "Vector.h"
#include "Curve.h"
#include "PixelMap.h"

static const int CANVAS_WIDTH = 256;
static const int CANVAS_HEIGHT = 160;

//static const std::array<float, 3> parallelThresholds = {0.2, 0.4, 0.75};
static const std::array<float, 9> parallelThresholds = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};

//static const std::array<float, 4> perpendicularThresholds = {5.5, 9.5, 18, 27};
static const std::array<float, 5> perpendicularThresholds = {5, 8.5, 11, 18, 27};

static const std::vector<std::vector<Curve::Cubic>> centerlines({
	{
		Curve::Cubic(51.2,16.6,	130.6,21.4,	203.6,68.3,	204.2,90.6),
		Curve::Cubic(204.2,90.6,	204.8,112.4,	180.6,143.2,	141.3,169.1),
	},
	{
		Curve::Cubic(224.6,126.7,	122.0,159.1,	63.9,145.7,	39.7,113.7),
		Curve::Cubic(39.7,113.7,	21.8,90.0,	16.8,30.9,	60.6,-11.9),
	},
	{
		Curve::Cubic(245.4,2.1,	230.1,83.9,	185.7,116.0,	148.7,121.9),
		Curve::Cubic(148.7,121.9, 119.7,126.4,	48.2,141.1,	-7.4,90.2),
	}
});

static const unsigned PALETTE_ANIM_FRAMES = 7;

struct TilemapEntry {
	unsigned short index: 10;
	bool flipx: 1;
	bool flipy: 1;
	unsigned short palette: 4;
};

struct Rgb555 {
	unsigned short red: 5;
	unsigned short green: 5;
	unsigned short blue: 5;

	static Rgb555 fromGreenBrightness(unsigned short brightness) {
		if (brightness > 31) {
			unsigned short brightness2 = brightness - 31;
			Rgb555 retval = {
				.red = brightness2,
				.green = 31,
				.blue = brightness2,
			};
			return retval;
		} else  {
			Rgb555 retval {
				.red = 0,
				.green = brightness,
				.blue = 0,
			};
			return retval;
		}
	}
};

class TileSet
{
private:
	std::vector<PixelMap<TILEDIM, TILEDIM>> tiles;
public:
	TileSet(void): tiles() {}

	std::pair<size_t, bool> insert(const PixelMap<TILEDIM, TILEDIM>& value)
	{
		size_t i = 0;
		for (auto t = tiles.cbegin(); t != tiles.cend(); t++, i++)
		{
			if (*t == value) {
				return std::make_pair(i, false);
			}
		}
		tiles.emplace_back(value);
		return std::make_pair(i, true);
	}

	bool writeTo(FILE* f)
	{
		for (auto i = tiles.cbegin(); i != tiles.cend(); i++)
		{
			bool wasWritten = i->writeTo(f);
			if (!wasWritten) return false;
		}
		return true;
	}
};

static int paletteIndexPerpendicular(double t, double distanceSq) {
	double x = (t < 0.5 ? t * 2 : (2 - t * 2));
	double x2 = (x < 0.5 ? x * x * 2 : x);

	size_t i = perpendicularThresholds.size();
	for (auto e = perpendicularThresholds.begin(); e != perpendicularThresholds.end(); e++, i--)
	{
		if (distanceSq < (*e) * (*e) * x2)
			return i;
	}
	return i;
}

static int paletteIndexParallel(double t) {
	size_t i = 0;
	for (auto e = parallelThresholds.begin(); e != parallelThresholds.end(); e++, i++)
	{
		if (t < *e)
			return i;
	}
	return i;
}

static uint8_t paletteIndex(double t, double distanceSq) {
	const int perp = paletteIndexPerpendicular(t, distanceSq);
	const int par = paletteIndexParallel(t);

	if (0 == perp)
		return 1;
	else
		return 2 + ((perp - 1) * (parallelThresholds.size() + 1) + par);
}


std::ostream& operator<<(std::ostream& os, const std::optional<double>& self)
{
	if (self)
	{
		os << "Some(" << self.value() << ")";
	}
	else
	{
		os << "None";
	}
	return os;
}


PixelMap<CANVAS_WIDTH, CANVAS_HEIGHT> drawSlash(const std::vector<Curve::Cubic>& centerline)
{
	PixelMap<CANVAS_WIDTH, CANVAS_HEIGHT> retval;

	for (int y = 0; y < CANVAS_HEIGHT; y++)
	for (int x = 0; x < CANVAS_WIDTH; x++)
	{
		Vector p(x, y);

		double distanceSquared = 1e100;
		double t = 0;

		for (unsigned i = 0; i < centerline.size(); i++) {
			Curve::Cubic part = centerline[i];
			std::optional<double> newTOpt = part.closestT(p);
			if (newTOpt) {
				double newT = newTOpt.value();
				double newDistanceSquared = p.distanceSq(part.apply(newT));
				if (newDistanceSquared < distanceSquared) {
					distanceSquared = newDistanceSquared;
					t = i + newT;
				}
			}
		}

		t /= centerline.size();

		retval.set(x, y, paletteIndex(t, distanceSquared));
	}

	return retval;
}

class BrightnessFunc {
public:
	BrightnessFunc() {}
	virtual ~BrightnessFunc() {}
	virtual float background() const =0;
	virtual float operator()(float perpNorm, float parNorm) const = 0;
};

class PeakBrightnessFunc : public BrightnessFunc {
public:
	PeakBrightnessFunc() {}
	~PeakBrightnessFunc() {}
	float background() const override {
		return 0.0f;
	}
	float operator()(float perpNorm, float parNorm) const override {
		return 62.0f * perpNorm;
	}
};

static PeakBrightnessFunc peakBrightnessFunc;

class IncreasingBrightnessFunc : public BrightnessFunc {
private:
	float progress;
public:
	IncreasingBrightnessFunc(float progress) : progress(progress) {}
	~IncreasingBrightnessFunc() {}
	float background() const override {
			return 0.0f;
	}
	float operator()(float perpNorm, float parNorm) const override {
		float parInv = 1.0f - parNorm;
		float progressAura = 2.0f * this->progress - 1.0f;

		float aura1 = -12.0f
			+ 31.0f * perpNorm
			+ 16.0f * parInv
			+ 31.0f * progressAura
			;
		float aura = std::max(this->background(), std::min(31.0f, aura1));

		float blade = 0;
		if (0.5 < this->progress) {
			float progressBlade = 2 * (this->progress - 0.5);
			blade = -20.0f
				+ 31.0f * perpNorm
				+ 31.0f * parInv
				+ 31.0f * progressBlade
				;
		}

		return std::min(peakBrightnessFunc(perpNorm, parNorm), std::max(aura, blade));
	}
};

class DecreasingBrightnessFunc : public BrightnessFunc {
private:
	float progress;
public:
	DecreasingBrightnessFunc(float progress) : progress(progress) {}
	~DecreasingBrightnessFunc() {}
	float background() const override {
			return 0.0f;
	}
	float operator()(float perpNorm, float parNorm) const override {
		float progressInv = 1.0f - this->progress;
		float progressAura = 2.0f * progressInv - 1.0f;

		float aura1 = -8.0f
			+ 31.0f * perpNorm
			+ 16.0f * parNorm
			+ 31.0f * progressAura
			;
		float aura = std::max(this->background(), std::min(31.0f, aura1));

		float blade = 0;
		if (0.5 > this->progress) {
			float progressBlade = 2 * (progressInv - 0.5);
			blade = -20.0f
				+ 31.0f * perpNorm
				+ 31.0f * parNorm
				+ 31.0f * progressBlade
				;
		}

		return std::min(peakBrightnessFunc(perpNorm, parNorm), std::max(aura, blade));
	}
};


static void printUsage(char* program_name)
{
	printf("%s tiles [0-%zd] tileset_filename tilemap_filename\n", program_name, centerlines.size() - 1);
	printf("%s palette [0-%d] palette_filename\n", program_name, PALETTE_ANIM_FRAMES * 2);
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printUsage(argv[0]);
	}
	else if (0 == strcmp("tiles", argv[1]) && argc == 5)
	{
		char *indexStr = argv[2];
		char *outtileset = argv[3];
		char *outtilemap = argv[4];

		size_t centerline_index = atoi(indexStr);

		auto canvas = drawSlash(centerlines[centerline_index]);
		TileSet tileset;
		std::vector<TilemapEntry> tilemap;
		for (auto i = canvas.tiles_begin(); i != canvas.tiles_end(); i++)
		{
			std::pair<size_t, bool> was_inserted = tileset.insert(*i);
			TilemapEntry tilemap_entry = {static_cast<unsigned short>(was_inserted.first), false, false, 0};
			tilemap.emplace_back(tilemap_entry);
		}

		FILE* f = fopen(outtileset, "w");
		tileset.writeTo(f);
		fclose(f);
		FILE* mapf = fopen(outtilemap, "w");
		fwrite(tilemap.data(), sizeof(TilemapEntry), tilemap.size(), mapf);
		fclose(mapf);
	}
	else if (0 == strcmp("palette", argv[1]) && argc == 4)
	{
		char *indexStr = argv[2];
		char *outpalette = argv[3];

		size_t index = atoi(indexStr);
		FILE* palf = fopen(outpalette, "w");

		BrightnessFunc *brightnessFunc;
		if (index < PALETTE_ANIM_FRAMES) {
			const float progress = (0.5f + index) / PALETTE_ANIM_FRAMES;
			brightnessFunc = new IncreasingBrightnessFunc(progress);
		} else if (index == PALETTE_ANIM_FRAMES) {
			brightnessFunc = new PeakBrightnessFunc();
		} else {
			const float progress = (-0.5f + index - PALETTE_ANIM_FRAMES) / PALETTE_ANIM_FRAMES;
			brightnessFunc = new DecreasingBrightnessFunc(progress);
		}

		{
			float brightnessFloat = brightnessFunc->background();
			unsigned short brightness = static_cast<unsigned short>(brightnessFloat);
			Rgb555 color = Rgb555::fromGreenBrightness(brightness);
			fwrite(&color, sizeof(Rgb555), 1, palf);
		}

		for (unsigned i = 0; i < perpendicularThresholds.size(); i++)
		for (unsigned j = 0; j < (1 + parallelThresholds.size()); j++)
		{
			float perpLo = (i == 0 ? 0.0 : perpendicularThresholds[i - 1]);
			float perpHi = (i == 0 ? 0.0 : perpendicularThresholds[i]);
			float perpMid = (perpHi - perpLo) / 2 + perpLo;
			float perpNorm = (perpMid / perpendicularThresholds.back());

			float parLo = (j == 0 ? 0.0 : parallelThresholds[j - 1]);
			float parHi = (j == parallelThresholds.size() ? 1.0 : parallelThresholds[j]);
			float parMid = (parHi - parLo) / 2 + parLo;

			float brightnessFloat = (*brightnessFunc)(perpNorm, parMid);
			unsigned short brightness = static_cast<unsigned short>(brightnessFloat);

			Rgb555 color = Rgb555::fromGreenBrightness(brightness);
			fwrite(&color, sizeof(Rgb555), 1, palf);
		}
		fclose(palf);
		delete brightnessFunc;
	}
	else
	{
		printUsage(argv[0]);
	}
}
