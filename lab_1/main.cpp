#include<iostream>
#include<stdint.h>
#include <random>
#include <algorithm> // Для std::shuffle
#include <vector>


typedef unsigned char u8;
typedef uint32_t      u32;

#define FALSE                  0
#define TRUE                   1
#define ERR_NOT_PTR            100
#define ERR_INVALID_INPUT_SIZE 101
#define ERR_INVALID_INPUT_CHAR 103


class BF
{
private:
    u32    nvar   = 0;
    u32    nbit   = 0;
    size_t sizebf = 0;
    u32*   bfunc  = nullptr;
public:
    BF(const int in_nvar, const u8 in_type);
    BF(const u8* in_str);
    BF(const BF& in_other);
    ~BF();

    u32 weight();
    float justice();


    friend std::ostream& operator << (std::ostream&, const BF&);
    friend std::istream& operator >> (std::istream&, BF&);
    void print_bf_nbit();
};


int gen_balanced_num(int in_nbits) {
    {
        std::random_device rd;
        std::default_random_engine engine(rd());

        /* Создание вектора с равным количеством единиц и нулей */
        std::vector<int> bits;
        bits.resize(in_nbits / 2, 0); /* Заполнение половины нулями */
        bits.resize(in_nbits, 1);     /* Заполнение второй половины единицами */

        /* Перемешивание вектора */
        std::shuffle(bits.begin(), bits.end(), engine);

        /* Преобразование в бинарное число */
        int res = 0;
        for (int bit : bits) {
            res = (res << 1) | bit;
        }

        return res;
    }
}

u32 error_accept(u32 in_errt) {
    switch (in_errt)
    {
    case ERR_NOT_PTR:
        std::cout << "Error null pointer! " << std::endl;
        exit(ERR_NOT_PTR);
        break;
    case ERR_INVALID_INPUT_SIZE:
        std::cout << "Error invalid input size! " << std::endl;
        exit(ERR_INVALID_INPUT_SIZE);
        break;
    case ERR_INVALID_INPUT_CHAR:
        std::cout << "Error invalid input symbol! " << std::endl;
        exit(ERR_INVALID_INPUT_CHAR);
        break;
    default:
        break;
    }

    return 0;
}

BF::BF(const int in_nvar, const u8 in_type) {
    /*
       реализация конструктора
       1 - 0
       2 - ввод
       3 - случайные
    */
    u32 i = 0;
    switch (in_type)
    {
        case 0:
            nvar = in_nvar;
            nbit = (1 << nvar)-1;
            sizebf = (nbit >> 5) + 1;
            bfunc = new u32[sizebf]();
            break;
        case 1:
            nvar = in_nvar;
            nbit = (1 << nvar)-1;
            sizebf = (nbit >> 5) + 1;
            bfunc = new u32[sizebf]();

            /* заполнение памяти */
            for (i = 0; i < nbit/32; ++i)
                bfunc[i] = ~bfunc[i];
            bfunc[i] = ~bfunc[i];
            bfunc[i] = bfunc[i] >> (32 - (nbit+1 % 32));
            
            break;
        case 2:
            nvar = in_nvar;
            nbit = (1 << nvar)-1;
            sizebf = (nbit >> 5) + 1;
            bfunc = new u32[sizebf]();
            
            /* заполнение памяти */
            for (i = 0; i < nbit/32; ++i)
                bfunc[i] = gen_balanced_num(32);
            bfunc[i] = gen_balanced_num(32);
            bfunc[i] = bfunc[i] >> (32 - (nbit+1 % 32));
        
            break;
        default:
            break;
    }
}

BF::BF(const u8* in_str){
    if (in_str == nullptr) {
        error_accept(ERR_NOT_PTR);
    }

    size_t N = std::strlen(reinterpret_cast<const char*>(in_str));
    u32 j = 0;
    u32 i = 2;
    for (; j < N; ++i) {
        j = 1 << i;
    }
    nbit   = j-1;
    nvar   = i - 2;
    sizebf = (nbit >> 5) + 1;
    bfunc  = new u32[sizebf]();

    for (u32 j = 0; j < N; j++)
    {
        int n_cel = j / 32;
        int n_bit = j % 32;

        if (in_str[j] == '1') {
            bfunc[n_cel] = bfunc[n_cel] | (1 << n_bit);
        }
        else if (in_str[j] == 0) {
            error_accept(ERR_INVALID_INPUT_SIZE);
        }
        else if (in_str[j] != '0') {
            error_accept(ERR_INVALID_INPUT_CHAR);
        }
    }
}

BF::BF(const BF& in_other) {
    nvar = in_other.nvar;
    nbit = in_other.nbit;
    sizebf = in_other.sizebf;
    bfunc = new u32[sizebf]();
    
    for (u32 i = 0; i < sizebf;++i) {
        bfunc[i] = in_other.bfunc[i];
    }
}

BF::~BF(){
    nvar   = 0;
    nbit   = 0;
    sizebf = 0;
    if(bfunc != nullptr)
        delete bfunc;
}

u32 BF::weight(){
    u32 copy;
    u32 res = 0;

    for (int i = 0; i < this->sizebf; ++i)
    {
        copy = this->bfunc[i];
        while (copy){
            copy &= copy-1;
            res++;
        }
    }
    return res;
}

float BF::justice(){
    return this->weight() / float(this->nbit+1);
}

std::ostream& operator<<(std::ostream& in_out, const BF& in_bfunc)
{
    for (u32 i = 0; i < in_bfunc.sizebf; ++i)
    {
        for (u32 n_bit = 0; n_bit < 32; ++n_bit) {
            in_out << (((1 << n_bit) & in_bfunc.bfunc[i]) ? '1' : '0');
        }
        in_out << " ";
    }

    return in_out;
}

std::istream& operator>>(std::istream& in_cin, BF& in_bfunc)
{
    std::string s = "";
    in_cin >> s;
    
    size_t len = s.length();
    if (in_bfunc.nbit < len-1){
        error_accept(ERR_INVALID_INPUT_SIZE);
    }
    
    u32 inptc = 0;
    for (u32 j = 0; j < len; j++)
    {
        int n_cel = j / 32;
        int n_bit = j % 32;

        if (s[j] == '1'){
            in_bfunc.bfunc[n_cel] = in_bfunc.bfunc[n_cel] | (1 << n_bit);
        }
        else if (s[j] == 0) {
            error_accept(ERR_INVALID_INPUT_SIZE);
        }
        else if (s[j] != '0') {
            error_accept(ERR_INVALID_INPUT_CHAR);
        }
        ++inptc;
    }
    if (len != inptc)
        error_accept(ERR_INVALID_INPUT_SIZE);
    return in_cin;
}

void BF::print_bf_nbit() {
    u32 t = nbit;
    for (u32 i = 0; i < sizebf; ++i)
    {
        for (u32 nbit = 0; (nbit < 32 && t != 0); ++nbit) {
            std::cout << (((1 << nbit) & bfunc[i]) ? '1' : '0');
            --t;
        }
        std::cout << " ";
    }
}

BF generate_justice_bfunc(u32 in_var)
{
    while (TRUE)
    {
        BF t    = BF(in_var, 2);
        u32 wei = t.weight();
        if ((wei <= 0.45) || (0.55 <= wei))
            return t;
    }
}

int main(void)
{
    {
        std::cout << "|------------------------------|" << std::endl;
        
        std::cout << "Case 1: BF(4, 0)" << std::endl;
        BF a = BF(4, 0);
        std::cout << "bfunc: ";
        std::cout << a << std::endl;
        std::cout << "bfunc weight: ";
        std::cout << a.weight() << std::endl;
        std::cout << "bfunc justice: ";
        std::cout << a.justice() << std::endl;

        std::cout << "|------------------------------|" << std::endl;
        std::cout << std::endl;
    }

    {
        std::cout << "|------------------------------|" << std::endl;
        
        std::cout << "Case 2: BF(4, 1)" << std::endl;
        BF a = BF(4, 1);
        std::cout << "bfunc: ";
        std::cout << a << std::endl;
        std::cout << "bfunc weight: ";
        std::cout << a.weight() << std::endl;
        std::cout << "bfunc justice: ";
        std::cout << a.justice() << std::endl;
        
        std::cout << "|------------------------------|" << std::endl;
        std::cout << std::endl;
    }

    {
        std::cout << "|------------------------------|" << std::endl;
        
        std::cout << "Case 3: BF(4, 2)" << std::endl;
        BF a = BF(4, 2);
        std::cout << "bfunc: ";
        std::cout << a << std::endl;
        std::cout << "bfunc weight: ";
        std::cout << a.weight() << std::endl;
        std::cout << "bfunc justice: ";
        std::cout << a.justice() << std::endl;

        std::cout << "|------------------------------|" << std::endl;
        std::cout << std::endl;
    }

    {
        std::cout << "|------------------------------|" << std::endl;

        std::cout << "Case 4: BF(6, 0)" << std::endl;
        BF a = BF(6, 0);
        std::cout << "bfunc: ";
        std::cout << a << std::endl;
        std::cout << "bfunc weight: ";
        std::cout << a.weight() << std::endl;
        std::cout << "bfunc justice: ";
        std::cout << a.justice() << std::endl;

        std::cout << "|------------------------------|" << std::endl;
        std::cout << std::endl;
    }

    {
        std::cout << "|------------------------------|" << std::endl;

        std::cout << "Case 5: BF(6, 1)" << std::endl;
        BF a = BF(6, 1);
        std::cout << "bfunc: ";
        std::cout << a << std::endl;
        std::cout << "bfunc weight: ";
        std::cout << a.weight() << std::endl;
        std::cout << "bfunc justice: ";
        std::cout << a.justice() << std::endl;

        std::cout << "|------------------------------|" << std::endl;
        std::cout << std::endl;
    }

    {
        std::cout << "|------------------------------|" << std::endl;

        std::cout << "Case 6: BF(6, 2)" << std::endl;
        BF a = BF(6, 2);
        std::cout << "bfunc: ";
        std::cout << a << std::endl;
        std::cout << "bfunc weight: ";
        std::cout << a.weight() << std::endl;
        std::cout << "bfunc justice: ";
        std::cout << a.justice() << std::endl;

        std::cout << "|------------------------------|" << std::endl;
        std::cout << std::endl;
    }

    {
        std::cout << "|------------------------------|" << std::endl;
        std::cout << "Case 7: BF(u32*)" << std::endl;
        
        u8 s[] = "11111111111111110111111111111111";
        std::cout << "u32*:  ";
        std::cout << s << std::endl;

        BF a = BF(s);
        std::cout << "bfunc: ";
        std::cout << a << std::endl;
        std::cout << "bfunc weight: ";
        std::cout << a.weight() << std::endl;
        std::cout << "bfunc justice: ";
        std::cout << a.justice() << std::endl;

        std::cout << "|------------------------------|" << std::endl;
        std::cout << std::endl;
    }

    {
        std::cout << "|------------------------------|" << std::endl;
        std::cout << "Case 8: BF(BF&)" << std::endl;

        BF b = BF(4,2);
        std::cout << "bfunc: ";
        std::cout << b << std::endl;
        std::cout << "bfunc weight: ";
        std::cout << b.weight() << std::endl;
        std::cout << "bfunc justice: ";
        std::cout << b.justice() << std::endl;

        BF a = BF(b);
        std::cout << "bfunc: ";
        std::cout << a << std::endl;
        std::cout << "bfunc weight: ";
        std::cout << a.weight() << std::endl;
        std::cout << "bfunc justice: ";
        std::cout << a.justice() << std::endl;

        std::cout << "|------------------------------|" << std::endl;
        std::cout << std::endl;
    }


    {
        std::cout << "|------------------------------|" << std::endl;

        std::cout << "Case 9: INPUT B(4, 0)" << std::endl;
        BF a = BF(4, 0);
        std::cin >> a;
        std::cout << "bfunc: ";
        std::cout << a << std::endl;
        std::cout << "bfunc weight: ";
        std::cout << a.weight() << std::endl;
        std::cout << "bfunc justice: ";
        std::cout << a.justice() << std::endl;
        std::cout << "OUTPUT: ";
        std::cout << a << std::endl;

        std::cout << "|------------------------------|" << std::endl;
        std::cout << std::endl;
    }

    {
        std::cout << "|------------------------------|" << std::endl;

        std::cout << "Case 10: gen justice for 5 var" << std::endl;
        BF a = generate_justice_bfunc(5);
        std::cout << "bfunc: ";
        std::cout << a << std::endl;
        std::cout << "bfunc weight: ";
        std::cout << a.weight() << std::endl;
        std::cout << "bfunc justice: ";
        std::cout << a.justice() << std::endl;
        std::cout << "OUTPUT: ";
        std::cout << a << std::endl;

        std::cout << "|------------------------------|" << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
/*
- Подсчет размера: ((1<<n)+31)>>5
- Получение случайного arrfun[i] = rand() - rand()
- Убрать последний бит:
    x   = 001100100
    x-1 = 001100011
    &   = 001100000
- wei = алгоритм убирать last bit while res not null

*/