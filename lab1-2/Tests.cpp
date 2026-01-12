#include "BigInt.h"
#include <iostream>
#include <chrono>

using namespace std;

void TestAll() {
    BigInt A = FromHex("9f3c8a71d4b29e6c8b5fa31d27c4e9a6");
    BigInt B = FromHex("7d1a9e8f63b0c4d52a79f8e6b3d2c1a0");
    BigInt N = FromHex("e95e4a5f737059dc60dfc7ad95b3d813");

    BigInt C, Q, R, G, L;
    BigInt diff;   
    BigInt sq;
    BigInt u;
    ComputeMu(N, u);

    cout << "A = " << ToHex(A) << endl;
    cout << "B = " << ToHex(B) << endl;


    LongAdd(A, B, C);
    cout << "A + B = " << ToHex(C) << "\n";

    if (LongCmp(A, B) >= 0) {
        LongSub(A, B, diff);
        cout << "A - B = " << ToHex(diff) << "\n";
    }
    else {
        LongSub(B, A, diff);
        cout << "A - B = -" << ToHex(diff) << "\n";
    }


    LongMul(A, B, C);
    cout << "A * B = " << ToHex(C) << "\n";

    LongDivMod(A, B, Q, R);
    cout << "A / B = " << ToHex(Q) << "\n";
    cout << "A mod B = " << ToHex(R) << "\n";

    BigInt two(2);
    LongPower(A, two, sq);
    cout << "A^2 = " << ToHex(sq) << "\n";

    LongGCD(A, B, G);
    cout << "gcd(A,B) = " << ToHex(G) << "\n";

    LongLCM(A, B, L);
    cout << "lcm(A,B) = " << ToHex(L) << "\n";

    LongAddMod(A, B, N, C);
    cout << "(A + B) mod N = " << ToHex(C) << "\n";

    LongSubMod(A, B, N, C);
    cout << "(A - B) mod N = " << ToHex(C) << "\n";

    LongSubMod(B, A, N, C);
    cout << "(B - A) mod N = " << ToHex(C) << "\n";

    LongMulMod(A, B, N, u, C);
    cout << "A * B mod N = " << ToHex(C) << "\n";

    LongSqrMod(A, N, u, C);
    cout << "A^2 mod N = " << ToHex(C) << "\n";


}

void BenchmarkAll() {
    const int iter = 1000;

    BigInt A = FromHex("9f3c8a71d4b29e6c8b5fa31d27c4e9a6");
    BigInt B = FromHex("7d1a9e8f63b0c4d52a79f8e6b3d2c1a0");
    BigInt C, Q, R;

    using clock = chrono::high_resolution_clock;

    auto bench = [&](const string& name, auto func) {
        auto t1 = clock::now();
        for (int i = 0; i < iter; ++i) func();
        auto t2 = clock::now();

        long long avg =
            chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count() / iter;

        cout << name << ": " << avg << " nanoseconds" << endl;
        };

    cout << "\nAverage time per operation:\n";

    bench("add", [&]() { LongAdd(A, B, C); });
    bench("sub", [&]() { LongSub(A, B, C); });
    bench("mul", [&]() { LongMul(A, B, C); });
    bench("div", [&]() { LongDivMod(A, B, Q, R); });

    BigInt e(2);
    bench("pow", [&]() { LongPower(A, e, C); });
}



int main() {
    TestAll();
    BenchmarkAll();
    return 0;
}