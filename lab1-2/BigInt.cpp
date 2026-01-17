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
        uint64_t carry = 0;
        for (int j = 0; j + i < N; j++) {
            uint64_t t = (uint64_t)A.a[j] * B.a[i] + C.a[i + j] + carry;
            C.a[i + j] = (uint32_t)(t & 0xFFFFFFFF);
            carry = t >> 32;
        }

        if (i + N < N) C.a[i + N] += (uint32_t)carry;
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

bool IsZero(const BigInt& A) {
    for (int i = 0; i < N; i++)
        if (A.a[i] != 0) return false;
    return true;
}


void LongDivMod(const BigInt& A, const BigInt& B, BigInt& Q, BigInt& R) {
    memset(Q.a, 0, sizeof(Q.a));
    R = BigInt(0);

    if (IsZero(B)) {
        return;
    }

    int n = BitLength(A);

    for (int i = n - 1; i >= 0; i--) {
    
        ShiftLeftBits(R, 1);

        
        if ((A.a[i / 32] >> (i % 32)) & 1)
            R.a[0] |= 1;

        if (LongCmp(R, B) >= 0) {
            LongSub(R, B, R);
            Q.a[i / 32] |= (1u << (i % 32));
        }
    }
}


void LongGCD(BigInt A, BigInt B, BigInt& G) {
    BigInt Q, R;
    while (!IsZero(B)) {
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


int NumDigits(const BigInt& A) {
    for (int i = N - 1; i >= 0; i--)
        if (A.a[i] != 0)
            return i + 1;
    return 1;
}


void ComputeMu(const BigInt& N, BigInt& u) {
    int k = NumDigits(N);

    BigInt beta2k(1);
    LongShiftDigitsToHigh(beta2k, 2 * k);

    BigInt q, r;
    LongDivMod(beta2k, N, q, r);
    u = q;
}



void BarrettReduction(const BigInt& x, const BigInt& n, const BigInt& u, BigInt& r) {
    int k = NumDigits(n);

    BigInt q1 = x;
    KillLastDigits(q1, k - 1);

    BigInt q2;
    LongMul(q1, u, q2);

    BigInt q3 = q2;
    KillLastDigits(q3, k + 1);

    BigInt tmp;
    LongMul(q3, n, tmp);
    LongSub(x, tmp, r); 

    while (LongCmp(r, n) >= 0) {
        LongSub(r, n, r);
    }
}

void LongAddMod(const BigInt& A, const BigInt& B, const BigInt& N, BigInt& C) {
    BigInt A1 = A, B1 = B;

    while (LongCmp(A1, N) >= 0) LongSub(A1, N, A1);
    while (LongCmp(B1, N) >= 0) LongSub(B1, N, B1);

    LongAdd(A1, B1, C);
    while (LongCmp(C, N) >= 0)
        LongSub(C, N, C);
}


void LongSubMod(const BigInt& A, const BigInt& B, const BigInt& N, BigInt& C) {
    BigInt A1 = A, B1 = B;

    if (LongCmp(A1, N) >= 0) LongSub(A1, N, A1);
    if (LongCmp(B1, N) >= 0) LongSub(B1, N, B1);

    if (LongCmp(A1, B1) >= 0)
        LongSub(A1, B1, C);
    else {
        BigInt t;
        LongSub(B1, A1, t);
        LongSub(N, t, C);
    }
}


void LongMulMod(const BigInt& A, const BigInt& B, const BigInt& N, const BigInt& u, BigInt& C) {
    BigInt P;
    LongMul(A, B, P);      
    BarrettReduction(P, N, u, C); 
}

void LongSqrMod(const BigInt& A, const BigInt& N, const BigInt& u, BigInt& C){
    BigInt S;
    LongMul(A, A, S);             
    BarrettReduction(S, N, u, C);  
}

void LongModPowerBarrett(const BigInt& A, const BigInt& B, const BigInt& N, BigInt& C) {
    BigInt u;
    ComputeMu(N, u);          

    BigInt base;
    BarrettReduction(A, N, u, base);
    C = BigInt(1);

    int m = BitLength(B);

    for (int i = 0; i < m; i++) {
        if ((B.a[i / 32] >> (i % 32)) & 1) {
            BigInt t;
            LongMul(C, base, t);
            BarrettReduction(t, N, u, C);
        }

        if (i < m - 1) {
            BigInt sq;
            LongMul(base, base, sq);
            BarrettReduction(sq, N, u, base);
        }
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