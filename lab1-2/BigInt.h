#pragma once

#include <cstdint>
#include <cstring>
#include <string>

using namespace std;

static const int N = 64;
static const uint64_t BASE = (1ULL << 32);

struct BigInt {
    uint32_t a[N];
    BigInt() { memset(a, 0, sizeof(a)); }
    BigInt(uint64_t x) {
        memset(a, 0, sizeof(a));
        a[0] = (uint32_t)x;
        a[1] = (uint32_t)(x >> 32);
    }
};

void LongAdd(const BigInt& A, const BigInt& B, BigInt& C);
void LongSub(const BigInt& A, const BigInt& B, BigInt& C);
int  LongCmp(const BigInt& A, const BigInt& B);

void LongMulOneDigit(const BigInt& A, uint32_t b, BigInt& C);
void LongShiftDigitsToHigh(BigInt& A, int k);
void LongMul(const BigInt& A, const BigInt& B, BigInt& C);

int  BitLength(const BigInt& A);
void ShiftLeftBits(BigInt& A, int k);
void LongDivMod(const BigInt& A, const BigInt& B, BigInt& Q, BigInt& R);

void LongPower(const BigInt& A, const BigInt& B, BigInt& C);

void LongGCD(BigInt A, BigInt B, BigInt& G);
void LongLCM(const BigInt& A, const BigInt& B, BigInt& L);

void KillLastDigits(BigInt& A, int t);
void BarrettReduction(const BigInt& x, const BigInt& n, const BigInt& u, BigInt& r);
void ComputeMu(const BigInt& N, BigInt& u);

void LongAddMod(const BigInt& A, const BigInt& B, const BigInt& N, BigInt& C);
void LongSubMod(const BigInt& A, const BigInt& B, const BigInt& N, BigInt& C);
void LongMulMod(const BigInt& A, const BigInt& B, const BigInt& N, const BigInt& u, BigInt& C);
void LongSqrMod(const BigInt& A, const BigInt& N, const BigInt& u, BigInt& C);
void LongModPowerBarrett(const BigInt& A, const BigInt& B, const BigInt& N, BigInt& C);


BigInt FromHex(const string& s);
string ToHex(const BigInt& X);
