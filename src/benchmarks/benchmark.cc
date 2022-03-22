#include "CountMinSketch/ModuloCountMinSketch.hh"
#include "CountMinSketch/MurmurCountMinSketch.hh"
#include "HDSketch/HDSketch.hh"
#include "utils/fasta.hh"
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <unordered_map>
#include <algorithm>
using namespace std;

template <typename T>
struct MurmurHash
{
    uint32_t operator()(const T& key) const
    {
        uint32_t result;
        MurmurHash3_x86_32(&key, sizeof(T), 417688365, &result);
        return result;
    }
};

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <fasta-file> <load-factor>" << endl;
        exit(1);
    }

    Fasta fa(argv[1]);
    double load_factor = strtod(argv[2], nullptr);
    size_t num_128mers = fa.size() - 127;

    cerr << "unordered map 0.5x ..." << endl;

    unordered_map<Compressed128Mer, int16_t, MurmurHash<Compressed128Mer>> dict;
    dict.reserve(2 * num_128mers);
    auto t0 = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < num_128mers; ++i)
    {
        Compressed128Mer key;
        fa.Read128Mer(i, key);
        dict[key] += 1;
    }
    auto t1 = chrono::high_resolution_clock::now();
    cout << "unordered map construction time: " << chrono::duration_cast<chrono::microseconds>(t1 - t0).count() << endl;

    vector<int16_t> out;
    out.reserve(num_128mers);

    {
        unordered_map<Compressed128Mer, int16_t, MurmurHash<Compressed128Mer>> dict_copy(dict);
        t0 = chrono::high_resolution_clock::now();
        for (const auto& it : dict_copy)
        {
            out.push_back(dict[it.first]);
        }
        t1 = chrono::high_resolution_clock::now();
        cout << "unordered map walk time: " << chrono::duration_cast<chrono::microseconds>(t1 - t0).count() << endl;
        swap(dict, dict_copy);
    }

    out.clear();



    cerr << "HDSketch " << load_factor << "x ..." << endl;
    random_device rd;
    mt19937_64 gen(rd());
    auto hd = new HDSketch<Compressed128Mer, int16_t>(num_128mers / load_factor, gen);

    t0 = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < num_128mers; ++i)
    {
        Compressed128Mer key;
        fa.Read128Mer(i, key);
        hd->insert(key);
    }
    t1 = chrono::high_resolution_clock::now();
    cout << "HDSketch " << load_factor << "x construct time: " << chrono::duration_cast<chrono::microseconds>(t1 - t0).count() << endl;

    out.reserve(num_128mers);

    t0 = chrono::high_resolution_clock::now();
    for (const auto& it : dict)
    {
        out.push_back(hd->estimate(it.first));
    }
    t1 = chrono::high_resolution_clock::now();
    cout << "HDSketch " << load_factor << "x walk time: " << chrono::duration_cast<chrono::microseconds>(t1 - t0).count() << endl;
    out.clear();

    size_t counter = 0;
    double square_err_sum = 0;
    for (const auto& it : dict)
    {
        ++counter;
        double err = hd->estimate(it.first) - it.second;
        square_err_sum += err * err;
    }
    cout << "HDSketch " << load_factor << "x MSE: " << square_err_sum / counter << endl;

    delete hd;
    hd = nullptr;


    // for (size_t i = 1; i <= 16; ++i)
    // {
    //     cerr << "ModuloCountMin (normal) " << load_factor << "x " << i << " rows ..." << endl;
    //     size_t height = i;
    //     size_t width = num_128mers / load_factor * 32 / height + 1;
    //     ModuloCountMinSketch<Compressed128Mer, int16_t> cms(width, height, gen);

    //     t0 = chrono::high_resolution_clock::now();
    //     for (size_t j = 0; j < num_128mers; ++j)
    //     {
    //         Compressed128Mer key;
    //         fa.Read128Mer(j, key);
    //         cms.insert(key);
    //     }
    //     t1 = chrono::high_resolution_clock::now();
    //     cout << "ModuloCountMin (normal) " << load_factor << "x " << i << " rows construct time: " << chrono::duration_cast<chrono::microseconds>(t1 - t0).count() << endl;

    //     out.reserve(num_128mers);
    //     t0 = chrono::high_resolution_clock::now();
    //     for (const auto& it : dict)
    //     {
    //         out.push_back(cms.estimate(it.first));
    //     }
    //     t1 = chrono::high_resolution_clock::now();
    //     cout << "ModuloCountMin (normal) " << load_factor << "x " << i << " rows walk time: " << chrono::duration_cast<chrono::microseconds>(t1 - t0).count() << endl;
    //     out.clear();

    //     counter = 0;
    //     square_err_sum = 0;
    //     for (const auto& it : dict)
    //     {
    //         ++counter;
    //         double err = cms.estimate(it.first) - it.second;
    //         square_err_sum += err * err;
    //     }
    //     cout << "ModuloCountMin (normal) " << load_factor << "x " << i << " rows MSE: " << square_err_sum / counter << endl;
    // }  


    for (size_t i = 1; i <= 16; ++i)
    {
        cerr << "ModuloCountMin (murmur) " << load_factor << "x " << i << " rows ..." << endl;
        size_t height = i;
        size_t width = num_128mers / load_factor * 32 / height + 1;
        MurmurCountMinSketch<Compressed128Mer, int16_t> cms(width, height, gen);

        t0 = chrono::high_resolution_clock::now();
        for (size_t j = 0; j < num_128mers; ++j)
        {
            Compressed128Mer key;
            fa.Read128Mer(j, key);
            cms.insert(key);
        }
        t1 = chrono::high_resolution_clock::now();
        cout << "ModuloCountMin (murmur) " << load_factor << "x " << i << " rows construct time: " << chrono::duration_cast<chrono::microseconds>(t1 - t0).count() << endl;

        out.reserve(num_128mers);
        t0 = chrono::high_resolution_clock::now();
        for (const auto& it : dict)
        {
            out.push_back(cms.estimate(it.first));
        }
        t1 = chrono::high_resolution_clock::now();
        cout << "ModuloCountMin (murmur) " << load_factor << "x " << i << " rows walk time: " << chrono::duration_cast<chrono::microseconds>(t1 - t0).count() << endl;
        out.clear();

        counter = 0;
        square_err_sum = 0;
        for (const auto& it : dict)
        {
            ++counter;
            double err = cms.estimate(it.first) - it.second;
            square_err_sum += err * err;
        }
        cout << "ModuloCountMin (murmur) " << load_factor << "x " << i << " rows MSE: " << square_err_sum / counter << endl;
    }  
}