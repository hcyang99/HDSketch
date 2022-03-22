#include "utils/fasta.hh"
#include <stdexcept>
#include <fstream>
using namespace std;

uint32_t compress16(const char* s)
{
    uint32_t result = 0;
    uint32_t shift = 0;
    for (int i = 0; i < 16; ++i)
    {
        uint32_t twoBitVal;
        switch(s[i]) 
        {
            case 'A': twoBitVal = 0; break;
            case 'C': twoBitVal = 1; break;
            case 'G': twoBitVal = 2; break;
            default: twoBitVal = 3; break;
        }
        result |= (twoBitVal << shift);
        shift += 2;
    }
    return result;
}

/**
 * @brief Compress input ACGT string each to 2-bit
 * @param dst Destination buffer
 * @param src Source string buffer
 */
void compressKernel(uint32_t* dst, const char* src, uint32_t num_outs)
{
    for (uint32_t i = 0; i < num_outs; ++i)
    {
        dst[i] = compress16(src + 16 * i);
    }
}

Fasta::Fasta(string path)
{
    string buffer;
    ifstream f(path);
    if (!f)
        throw runtime_error("Cannot open file");
    
    string line;
    getline(f, line);
    while (true)
    {
        getline(f, line);
        if (line.size() != 0 && line[0] != '>' && line[0] != ';')
        {   
            buffer += line;
        }
        if (f.eof() || (line.size() != 0 && line[0] == '>'))
            break;
    }
    f.close();

    this->sz = buffer.size();

    int remainder = buffer.size() % 16;
    int pad = remainder == 0 ? 0 : 16 - remainder;

    // pad with 'A' for compressing
    buffer += string(pad, 'A');

    compressed = new uint32_t[buffer.size() / 4];
    compressKernel(compressed, &buffer[0], buffer.size() / 16);
}

void Fasta::Read128Mer(uint32_t offset, Compressed128Mer& out) const
{
    for (int i = 0; i < 8; ++i)
    {
        uint32_t index = offset / 16 + i;
        uint32_t shift_1 = offset % 16 * 2;
        uint32_t shift_2 = (16 - shift_1) * 2;
        out.u32[i] = (compressed[index] >> shift_1) | (compressed[index + 1] << shift_2);
    }
}

