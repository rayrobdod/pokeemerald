#include <iterator>
#define TILEDIM (8)

template<unsigned int WIDTH, unsigned int HEIGHT>
class PixelMap
{
private:
	uint8_t* data;
public:
	PixelMap(void)
	{
		this->data = new uint8_t[WIDTH * HEIGHT];
	}
	PixelMap(const PixelMap& other) {
		this->data = new uint8_t[WIDTH * HEIGHT];
		for (unsigned j = 0; j < HEIGHT; j++)
		for (unsigned i = 0; i < WIDTH; i++)
		{
			this->set(i, j, other.get(i, j));
		}
	}
	~PixelMap(void)
	{
		delete[] data;
	}
	void set(size_t x, size_t y, uint8_t newValue)
	{
		this->data[y * WIDTH + x] = newValue;
	}
	uint8_t get(size_t x, size_t y) const {
		return this->data[y * WIDTH + x];
	}

	bool writeTo(FILE* f) const
	{
		size_t num = fwrite(this->data, sizeof(uint8_t), WIDTH * HEIGHT, f);
		return (WIDTH * HEIGHT) == num;
	}

	bool operator==(const PixelMap<WIDTH, HEIGHT>& other ) const
	{
		for (unsigned y = 0; y < HEIGHT; y++)
		for (unsigned x = 0; x < WIDTH; x++)
		{
			if (this->get(x, y) != other.get(x, y))
				return false;
		}
		return true;
	}

	class tiles_iterator {
	private:
		PixelMap<WIDTH, HEIGHT>* enclose;
		unsigned x;
		unsigned y;
	public:
		explicit tiles_iterator(PixelMap<WIDTH, HEIGHT>* enclose) : enclose(enclose), x(0), y(0) {}
		explicit tiles_iterator(PixelMap<WIDTH, HEIGHT>* enclose, unsigned x, unsigned y) : enclose(enclose), x(x), y(y) {}
		tiles_iterator& operator++()
		{
			this->x += TILEDIM;
			if (this->x >= WIDTH)
			{
				this->x = 0;
				this->y += 8;
			}
			return *this;
		}
		tiles_iterator operator++(int) { tiles_iterator retval = *this; ++(*this); return retval; }
		bool operator==(tiles_iterator other) const { return this->x == other.x && this->y == other.y; }
		bool operator!=(tiles_iterator other) const { return !(*this == other); }
		PixelMap<TILEDIM, TILEDIM> operator*() const {
			PixelMap<TILEDIM, TILEDIM> retval;
			for (unsigned j = 0; j < TILEDIM; j++)
			for (unsigned i = 0; i < TILEDIM; i++)
			{
				retval.set(i, j, this->enclose->get(i + this->x, j + this->y));
			}
			return retval;
		}
	};

	tiles_iterator tiles_begin() {
		return tiles_iterator(this);
	}

	tiles_iterator tiles_end() {
		return tiles_iterator(this, 0, HEIGHT / 8 * 8);
	}
};
