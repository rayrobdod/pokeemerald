#include "wordinfo.h"

#include "fatal_error.h"
#include "util.h"
#include <algorithm>
#include <iostream>

extern "C" {
typedef unsigned short u16;
u16 SpeciesToHoennPokedexNum(u16 species);
u16 SpeciesToNationalPokedexNum(u16 species);

#include "constants/species.h"
#include "constants/pokedex.h"

typedef char u8;
#define POKEMON_NAME_LENGTH 20
#define _(x) {x}
#include "../../src/data/text/species_names.h"

#include "constants/easy_chat.h"
}

using std::string;

struct PokemonGroupData
{
    std::string id;
    std::string label;
    u16 pokemonCount;
};

static const struct PokemonGroupData hoennPokemonGroupData
{
    .id = "EC_GROUP_POKEMON",
    .label = "gEasyChatGroup_Pokemon",
    .pokemonCount = HOENN_DEX_COUNT,
};
static const struct PokemonGroupData kantoJohtoPokemonGroupData
{
    .id = "EC_GROUP_POKEMON_NATIONAL",
    .label = "gEasyChatGroup_Pokemon2",
    .pokemonCount = JOHTO_DEX_COUNT,
};

static const PokemonGroupData& pokemonGroupData(DexGroup dex_group)
{
    switch (dex_group)
    {
    case DexGroup::HOENN:
        return hoennPokemonGroupData;
    case DexGroup::KANTOJOHTO:
        return kantoJohtoPokemonGroupData;
    }
    FATAL_ERROR("illegal argument\n");
}

WordInfoList listPokemon(DexGroup dex_group)
{
    const PokemonGroupData& dex_group_data = pokemonGroupData(dex_group);

    std::vector<WordInfo> words_out;

    for (unsigned i = 0; i < NUM_SPECIES; i++)
    {
        u16 dexNo;
        switch (dex_group)
        {
        case DexGroup::HOENN:
            dexNo = SpeciesToHoennPokedexNum(i);
            break;
        case DexGroup::KANTOJOHTO:
            dexNo = SpeciesToNationalPokedexNum(i);
            break;
        }

        if (dexNo && dexNo <= dex_group_data.pokemonCount)
        {
            string name(gSpeciesNames[i]);

            string id("(");
            id += dex_group_data.id;
            id += " << EC_MASK_BITS) | ";
            id += std::to_string(i);
            id += " /* ";
            id += name;
            id += " */";

            string index("");
            index += std::to_string(i);
            index += " /* ";
            index += name;
            index += " */";

            words_out.emplace_back(
                id,
                index,
                name,
                "",
                true);
        }
    }

    WordInfoList retval(
        dex_group_data.id,
        dex_group_data.label,
        WordInfoListType::VALUE_LIST,
        words_out);
    return retval;
}
