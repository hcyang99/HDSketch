#include "BehavioralHD/RandomModelHD.hh"
#include <iostream>
#include <vector>
using namespace std;

template <size_t D>
double gen_square_err(size_t conflicts, mt19937_64& gen)
{
    using RandHD = RandomModelHD<int16_t, D>;
    const int multiplicity = 16;

    ModelHD<int16_t, D> class_hv;
    RandHD query_hv(gen);

    class_hv += query_hv;
    class_hv *= multiplicity;

    for (size_t i = 0; i < conflicts; ++i)
    {
        RandHD noise(gen);
        class_hv += noise;
    }

    double res = class_hv.dot(query_hv) / D;
    double square_error = res - multiplicity;
    square_error *= square_error;

    return square_error;
}

template <size_t D>
double run(mt19937_64& gen)
{
    double sum_square_err = 0;
    const int times = 10000;
    const size_t conflicts = 32;

    for (int j = 0; j < times; ++j)
    {
        sum_square_err += gen_square_err<D>(conflicts, gen);
    }
    double mse = sum_square_err / times;
    return mse;
}

int main()
{
    random_device rd;
    mt19937_64 gen(rd());

    constexpr size_t dimensions[] = {16, 32, 64, 128, 256, 512, 1024};
    vector<double> mse;

    mse.push_back(run<dimensions[0]>(gen));
    mse.push_back(run<dimensions[1]>(gen));
    mse.push_back(run<dimensions[2]>(gen));
    mse.push_back(run<dimensions[3]>(gen));
    mse.push_back(run<dimensions[4]>(gen));
    mse.push_back(run<dimensions[5]>(gen));
    mse.push_back(run<dimensions[6]>(gen));

    for (int i = 0; i < mse.size(); ++i)
    {
        cout << mse[i] << ", ";
    }
    cout << endl;
}