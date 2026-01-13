#include "BigInt.h"
#include <sstream>
#include <iomanip>

using namespace std;

void LongAdd(const BigInt& A, const BigInt& B, BigInt& C) {
    uint32_t carry = 0;
    for (int i = 0; i < N; i++) {
        uint64_t t = (uint64_t)A.a[i] + B.a[i] + carry;
        C.a[i] = (uint32_t)(t & (BASE - 1));
        carry = (uint32_t)(t >> 32);
    }
}


void LongSub(const BigInt& A, const BigInt& B, BigInt& C) {
    uint32_t borrow = 0;
    for (int i = 0; i < N; i++) {
        int64_t t = (int64_t)A.a[i] - B.a[i] - borrow;
        if (t >= 0) {
            C.a[i] = (uint32_t)t;
            borrow = 0;
        }
        else {
            C.a[i] = (uint32_t)(t + BASE);
            borrow = 1;
        }
    }
}

int LongCmp(const BigInt& A, const BigInt& B) {
    for (int i = N - 1; i >= 0; i--) {
        if (A.a[i] > B.a[i]) return 1;
        if (A.a[i] < B.a[i]) return -1;
    }
    return 0;
}

void LongMulOneDigit(const BigInt& A, uint32_t b, BigInt& C) {
    uint64_t carry = 0;
    for (int i = 0; i < N; i++) {
        uint64_t t = (uint64_t)A.a[i] * b + carry;
        C.a[i] = (uint32_t)(t & (BASE - 1));
        carry = t >> 32;
    }
}


void LongShiftDigitsToHigh(BigInt& A, int k) {
    if (k <= 0) return;
    for (int i = N - 1; i >= k; i--)
        A.a[i] = A.a[i - k];
    for (int i = 0; i < k; i++)
        A.a[i] = 0;
}


void LongMul(const BigInt& A, const BigInt& B, BigInt& C) {
    memset(C.a, 0, sizeof(C.a));

    for (int i = 0; i < N; i++) {
        BigInt temp;
        LongMulOneDigit(A, B.a[i], temp);
        LongShiftDigitsToHigh(temp, i);

        BigInt sum;
        LongAdd(C, temp, sum);
        C = sum;
    }
}


int BitLength(const BigInt& A) {
    for (int i = N - 1; i >= 0; i--) {
        if (A.a[i]) {
            for (int b = 31; b >= 0; b--)
                if (A.a[i] & (1u << b))
                    return i * 32 + b + 1;
        }
    }
    return 0;
}


void ShiftLeftBits(BigInt& A, int k) {
    while (k--) {
        uint32_t carry = 0;
        for (int i = 0; i < N; i++) {
            uint64_t t = ((uint64_t)A.a[i] << 1) | carry;
            A.a[i] = (uint32_t)t;
            carry = (uint32_t)(t >> 32);
        }
    }
}


void LongDivMod(const BigInt& A, const BigInt& B, BigInt& Q, BigInt& R) {
    memset(Q.a, 0, sizeof(Q.a));
    int k = BitLength(B);
    R = A;
    while (LongCmp(R, B) >= 0) {
        int t = BitLength(R);
        BigInt C = B;
        ShiftLeftBits(C, t - k);
        if (LongCmp(R, C) < 0) {
            t--;
            C = B;
            ShiftLeftBits(C, t - k);
        }
        LongSub(R, C, R);
        Q.a[(t - k) / 32] |= (1u << ((t - k) % 32));
    }
}

void LongGCD(BigInt A, BigInt B, BigInt& G) {
    BigInt Q, R;

    while (!(B.a[0] == 0 && memcmp(B.a, BigInt().a, sizeof(B.a)) == 0)) {
        LongDivMod(A, B, Q, R);
        A = B;
        B = R;
    }
    G = A;
}


void LongLCM(const BigInt& A, const BigInt& B, BigInt& L) {
    BigInt G, T, Q, R;

    LongGCD(A, B, G);   
    LongMul(A, B, T);    
    LongDivMod(T, G, Q, R); 

    L = Q;
}

void KillLastDigits(BigInt& A, int t) {
    if (t <= 0) return;
    if (t >= N) {
        memset(A.a, 0, sizeof(A.a));
        return;
    }

    for (int i = 0; i < N - t; i++)
        A.a[i] = A.a[i + t];

    for (int i = N - t; i < N; i++)
        A.a[i] = 0;
}

void ComputeMu(const BigInt& N, BigInt& u) {
    BigInt one(1);
    u = one;

    int k = BitLength(N) / 32 + 1;
    LongShiftDigitsToHigh(u, 2 * k);   

    BigInt q, r;
    LongDivMod(u, N, q, r);
    u = q;                          
}


void BarrettReduction(const BigInt& x, const BigInt& n, const BigInt& u, BigInt& r) {
    BigInt q = x;
    int k = BitLength(n) / 32 + 1;

    KillLastDigits(q, k - 1);

    BigInt tmp;
    LongMul(q, u, tmp);
    q = tmp;

    KillLastDigits(q, k + 1);

    LongMul(q, n, tmp);
    LongSub(x, tmp, r);

    while (LongCmp(r, n) >= 0)
        LongSub(r, n, r);
}

void LongAddMod(const BigInt& A, const BigInt& B, const BigInt& N, BigInt& C) {
    BigInt S;
    LongAdd(A, B, S);         

    if (LongCmp(S, N) >= 0)    
        LongSub(S, N, C);     
    else
        C = S;
}


void LongSubMod(const BigInt& A, const BigInt& B, const BigInt& N, BigInt& C) {
    if (LongCmp(A, B) >= 0)
    {
        LongSub(A, B, C);     
    }
    else
    {
        BigInt t;
        LongSub(B, A, t);      
        LongSub(N, t, C);      
    }
}


void LongMulMod(const BigInt& A,
    const BigInt& B,
    const BigInt& N,
    const BigInt& u,
    BigInt& C)
{
    BigInt P;
    LongMul(A, B, P);               
    BarrettReduction(P, N, u, C);   
}

void LongSqrMod(const BigInt& A,
    const BigInt& N,
    const BigInt& u,
    BigInt& C)
{
    BigInt S;
    LongMul(A, A, S);             
    BarrettReduction(S, N, u, C);  
}

void LongModPowerBarrett(const BigInt& A, const BigInt& B, const BigInt& N, BigInt& C) {
    BigInt u;
    ComputeMu(N, u);          

    BigInt base = A;
    C = BigInt(1);

    int m = BitLength(B);

    for (int i = 0; i < m; i++) {
        if ((B.a[i / 32] >> (i % 32)) & 1) {
            BigInt t;
            LongMul(base, C, t);
            BarrettReduction(t, N, u, C);
        }

        BigInt sq;
        LongMul(base, base, sq);
        BarrettReduction(sq, N, u, base);
    }
}



void LongPower(const BigInt& A, const BigInt& B, BigInt& C) {
    BigInt base = A;
    C = BigInt(1);

    for (int i = 0; i < N * 32; i++) {
        if ((B.a[i / 32] >> (i % 32)) & 1) {
            BigInt t;
            LongMul(C, base, t);
            C = t;
        }
        BigInt sq;
        LongMul(base, base, sq);
        base = sq;
    }
}


BigInt FromHex(const string& s) {
    BigInt x;
    int pos = 0;
    for (int i = (int)s.size(); i > 0; i -= 8) {
        int start = max(0, i - 8);
        x.a[pos++] = stoul(s.substr(start, i - start), nullptr, 16);
    }
    return x;
}

string ToHex(const BigInt& X) {
    ostringstream ss;
    bool started = false;
    for (int i = N - 1; i >= 0; i--) {
        if (!started) {
            if (X.a[i] == 0) continue;
            ss << hex << X.a[i];
            started = true;
        }
        else {
            ss << setw(8) << setfill('0') << hex << X.a[i];
        }
    }
    return started ? ss.str() : "0";
}