#pragma once
#include "utils.hh"
#include <string>

using utils::Compressed128Mer;

class Fasta
{
    public:
    /**
     * @brief Constructor, reads fasta to CPU memory
     * @param path Path to fasta file
     */
    Fasta(std::string path);

    /**
     * @brief Reads Compressed128Mer
     * @param offset offset of the 128-mer
     * @param out output  
     */
    void Read128Mer(uint32_t offset, Compressed128Mer& out) const;

    size_t size() const {return sz;}

    protected:
    size_t sz;
    uint32_t* compressed;
};
