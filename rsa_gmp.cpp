#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string>

#include <gmp.h>
#include "PrimeGenerator.h"

using namespace std;

void gcd(mpz_t result, mpz_t a, mpz_t b)
{
    mpz_t tmp;
    mpz_init(tmp);
    for (;;)
    {
        mpz_mod(tmp, a, b);
        if (mpz_cmp_ui(tmp, 0) == 0){
            mpz_set(result, b);
        }
        mpz_set(a, b);
        mpz_set(b, tmp);
    }
}

void rsaenc(mpz_t enck, char &val, mpz_t e, mpz_t p, mpz_t q)
{
    
    mpz_t phi, n;
    mpz_init(phi);
    mpz_init(n);

    // set p - 1 and q - 1
    mpz_t ptmp, qtmp;
    mpz_init(ptmp);
    mpz_init(qtmp);
    mpz_sub_ui(ptmp, p, 1);
    mpz_sub_ui(qtmp, q, 1);

    // calculate phi
    mpz_mul(ptmp, ptmp, qtmp);
    mpz_set(phi, ptmp);

    mpz_mul(n, p, q);

    // setting o to the double char value
    mpz_t o;
    mpz_init(o);
    mpz_set_ui(o, int(val));

    // encryption key
    mpz_init(enck);
    // NOTE: Cannot raise to mpz_t power (too big anyway)
    mpz_pow_ui(enck, o, mpz_get_ui(e));

    mpf_t d1, tmp;
    mpf_init(d1);
    mpf_init(tmp);
    mpf_set_d(tmp, -1);
    mpf_div_ui(d1, tmp, mpz_get_ui(e));
    
    //private key
    mpf_t d, tmpPhi;
    mpf_init(d);
    mpf_init(tmpPhi);
    mpf_set(d, d1);
    mpf_set_z(tmpPhi, phi);    
    while (mpf_cmp_ui(d, 0) >= 0){
        mpf_sub(d, d, tmpPhi);
    }
    mpf_add(d, d, tmpPhi);
    mpf_add(d, d, tmpPhi);


    mpz_t deck;
    mpz_init(deck);

    //decryption key
    mpz_pow_ui(deck, enck, mpf_get_d(d));

    //encrypt the message
    // ?????
    mpz_t enc;
    mpz_init(enc);
    std::cout << mpz_get_ui(n) << std::endl;

    mpz_mod(enc, enck, n);

    val = char(mpz_get_ui(enc));

    cout << "\n"
         << "p = " << p << "\nq = " << q;
    cout << "\n"
         << "n=p*q = " << n << "\nphi = " << phi;
    cout << "\n"
         << "e = " << e << "\nd = " << d;
    cout << "\n--------------------------------------------------------------";
    cout << "\n"
         << "Encrypted value = " << enc;
    cout << "\n"
         << "Encrypted char = " << val;
    cout << "\n--------------------------------------------------------------";

}

void rsadec(char &val, mpz_t e, mpz_t p, mpz_t q, mpz_t enck)
{
    mpz_t phi, n;
    mpz_init(phi);
    mpz_init(n);

    // set p - 1 and q - 1
    mpz_t ptmp, qtmp;
    mpz_init(ptmp);
    mpz_init(qtmp);
    mpz_sub_ui(ptmp, p, 1);
    mpz_sub_ui(qtmp, q, 1);

    // calculate phi
    mpz_mul(ptmp, ptmp, qtmp);
    mpz_set(phi, ptmp);

    mpz_mul(n, p, q);

    mpf_t d1, tmp;
    mpf_init(d1);
    mpf_init(tmp);
    mpf_set_d(tmp, -1);
    mpf_div_ui(d1, tmp, mpz_get_ui(e));

    //private key
    mpf_t d, tmpPhi;
    mpf_init(d);
    mpf_init(tmpPhi);
    mpf_set(d, d1);
    mpf_set_z(tmpPhi, phi);    
    while (mpf_cmp_ui(d, 0) >= 0){
        mpf_sub(d, d, tmpPhi);
    }
    mpf_add(d, d, tmpPhi);
    mpf_add(d, d, tmpPhi);

    mpz_t deck;
    mpz_init(deck);

    //decryption key
    mpz_pow_ui(deck, enck, mpf_get_d(d));


    mpz_t dec;
    mpz_init(dec);
    mpz_mod(dec, deck, n);

    char de = char(mpz_get_ui(dec));

    cout << "\n"
         << "p = " << p << "\t\tq = " << q;
    cout << "\n"
         << "n=p*q = " << n << "\tphi = " << phi;
    cout << "\n"
         << "e = " << e << "\t\td = " << d;
    cout << "\n--------------------------------------------------------------";
    cout << "\n"
         << "Decrypted value = " << dec;
    cout << "\n"
         << "Decrypted char = " << de;
    cout << "\n--------------------------------------------------------------";
}

void pubkey(mpz_t result, mpz_t p, mpz_t q)
{
    // random state
    gmp_randstate_t randState;
    gmp_randinit_default(randState);

    // declare variables
    mpz_t phi, track;
    mpz_init(result);
    mpz_init(phi);
    mpz_init(track);

    // set e to rand
    mpz_urandomb(result, randState, 10);
    
    // set p - 1 and q - 1
    mpz_t ptmp, qtmp;
    mpz_init(ptmp);
    mpz_init(qtmp);
    mpz_sub_ui(ptmp, p, 1);
    mpz_sub_ui(qtmp, q, 1);

    // calculate phi
    mpz_mul(ptmp, ptmp, qtmp);
    mpz_set(phi, ptmp);
    
    while (result < phi)
    {
        //to check e and phi(n) are coprime.
        gcd(track, result, phi);
        if (mpz_cmp_ui(track, 1) == 0)
        {
            break;
        }
        else
        {
            mpz_add_ui(result, result, 1);
        }
    }
    // returns public key in 'result' variable
}
int main()
{
    mpz_t p, q;
    mpz_init(p);
    mpz_init(q);
    PrimeGenerator* pg = new PrimeGenerator();
    pg->setNumBits(600);
    pg->getPrimeNumber(p);
    pg->setNumBits(700);
    pg->getPrimeNumber(q);

    string orig, de, en;
    mpz_t e;
    mpz_init(e);

    pubkey(e, p, q);
    cout << "Enter the message : ";
    getline(cin, orig);
    de = en = orig;
    de = "";

    mpz_t k;
    mpz_init(k);

    for (int i = 0; i < orig.length(); i++)
        rsaenc(k, en[i], e, p, q);
    for (int i = 0; i < orig.length(); i++)
        rsadec(de[i], e, p, q, k);
    cout << "\nThe encrypted text = " << en;
    cout << "\nThe decrypted text = " << de;
    return 0;
}