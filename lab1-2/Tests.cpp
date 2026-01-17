#include "BigInt.h"
#include <iostream>
#include <chrono>
#include <iomanip>


using namespace std;

void TestAll() {
    BigInt B = FromHex("27d7d52459bf8af69336f500c06cf172134c255f928f6bccab7951146a7e9d86c092c1837cd6cebee2cf737b83bfe4bb24133981fbd068c374f7b909c08b9930b14f026728531ccf3ad1196e2b905ac025ede9eedb09a58905f8d48ea5580fa99744ac8104cad95f9bc128525bca142e2a587e723f9e1fed3803c915b7cbcb43");
    BigInt A = FromHex("2136201536c97ff9f847d651198760b707804708923ca2673cfa2103f229fd61530f97b0b6eb8e128201b722bdafadf7f575f7be64a1c50eb149f5ddc3d5cbb83afe1be2b87b54002e0b86fd72511e093945840381bc5fa08519e21c70b98e7695266a223071c8c66b67c201bcd6e77b6b4b73e35fd01ad1b6008a47a0625e81");
    BigInt N = FromHex("aa7ae6683168a693bf5f463943de0dd25f6e12ffb5e02fb337495fef83a6bd9687e48cc70cb25f3328294dbfd3601d3e4580eb0a062df6196ba923dae764818791befd11046f125b3b7eee30a7b1d8b67fa00122304623cb5203bdec121ead4d941d7b58de2e56ddf12a6ef86c1335f6f891ce5f4789965b65e5cc3d751ab75");

    BigInt C, Q, R, G, L;
    BigInt diff;   
    BigInt sq;
    BigInt u;
    ComputeMu(N, u);

    cout << "A = " << ToHex(A) << "\n";
    cout << "B = " << ToHex(B) << "\n";
    cout << "N = " << ToHex(A) << "\n";

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

    LongModPowerBarrett(A, B, N, C);
    cout << "A^B mod N = " << ToHex(C) << "\n";

    BigInt l, r;
    BigInt t1, t2, sum;

    LongAddMod(A, B, N, sum);
    LongMulMod(sum, A, N, u, l);

    LongMulMod(A, A, N, u, t1);
    LongMulMod(B, A, N, u, t2);
    LongAddMod(t1, t2, N, r);

    cout << "(A + B) * A mod N = " << ToHex(l) << "\n";
    cout << "A*A + B*A mod N = " << ToHex(r) << "\n";

    if (LongCmp(l, r) == 0)
        cout << "Distributivity test: OK\n";
    else
        cout << "Distributivity test: ERROR\n";

}

void BenchmarkAll() {
    const int iter = 1000;

    BigInt A = FromHex("27d7d52459bf8af69336f500c06cf172134c255f928f6bccab7951146a7e9d86c092c1837cd6cebee2cf737b83bfe4bb24133981fbd068c374f7b909c08b9930b14f026728531ccf3ad1196e2b905ac025ede9eedb09a58905f8d48ea5580fa99744ac8104cad95f9bc128525bca142e2a587e723f9e1fed3803c915b7cbcb43");
    BigInt B = FromHex("2136201536c97ff9f847d651198760b707804708923ca2673cfa2103f229fd61530f97b0b6eb8e128201b722bdafadf7f575f7be64a1c50eb149f5ddc3d5cbb83afe1be2b87b54002e0b86fd72511e093945840381bc5fa08519e21c70b98e7695266a223071c8c66b67c201bcd6e77b6b4b73e35fd01ad1b6008a47a0625e81");
    BigInt N = FromHex("aa7ae6683168a693bf5f463943de0dd25f6e12ffb5e02fb337495fef83a6bd9687e48cc70cb25f3328294dbfd3601d3e4580eb0a062df6196ba923dae764818791befd11046f125b3b7eee30a7b1d8b67fa00122304623cb5203bdec121ead4d941d7b58de2e56ddf12a6ef86c1335f6f891ce5f4789965b65e5cc3d751ab75");


    BigInt C, Q, R, mu;

    ComputeMu(N, mu);

    using clock = chrono::high_resolution_clock;

    auto bench = [&](const string& name, auto func) {
        auto t1 = clock::now();
        for (int i = 0; i < iter; ++i) func();
        auto t2 = clock::now();

        long long avg =
            chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count() / iter;

        cout << setw(15) << left << name
            << avg << " ns" << endl;
        };

    cout << "\nAverage time per operation:\n";
    cout << "------------------------------------\n";

    bench("add", [&]() { LongAdd(A, B, C); });
    bench("sub", [&]() { LongSub(A, B, C); });
    bench("mul", [&]() { LongMul(A, B, C); });
    bench("div", [&]() { LongDivMod(A, B, Q, R); });

    bench("add mod", [&]() { LongAddMod(A, B, N, C); });
    bench("sub mod", [&]() { LongSubMod(A, B, N, C); });
    bench("mul mod", [&]() { LongMulMod(A, B, N, mu, C); });
    bench("square mod", [&]() { LongSqrMod(A, N, mu, C); });

    BigInt E = FromHex("0102030405");
    bench("mod power", [&]() {
        LongModPowerBarrett(A, E, N, C);
        });
}




int main() {
    TestAll();
    BenchmarkAll();
    return 0;
}