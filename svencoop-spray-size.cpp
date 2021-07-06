#include <iostream>
#include <set>
#include <vector>
#include <cmath>
#define NUMTIMESDEN 56

using namespace std;

// greatest common denominator of any 2 uints
unsigned gcd(unsigned a, unsigned b) {return (b == 0) ? a : gcd(b, a % b);}

// a fraction class with limited functionality to represent fractions
class Fraction
{
        unsigned num, den;
    public:
        Fraction (unsigned n, unsigned d){
            // constructor will default to 0/1 if the denominator is 0
            if (d == 0){num = 0; den = 1;}
            // otherwise make sure the fraction is in its simplest form (150/100 -> 3/2, for example)
            else{num = n / gcd(n,d);den = d / gcd(n,d);}
            // weirdly enough, instead of an int division by zero exception, a floating point one happened to me
            // no floats in my code
        }
        Fraction (){
            num = 0; den = 1;
        }
        // these 2 return the numerator and the denominator
        unsigned getNum() const {return num;}
        unsigned getDen() const {return den;}
        // boolean operators, needed mostly for the ordered set of ratios
        bool operator==(const Fraction rhs) const {return (getNum() == rhs.getNum() && getDen() == rhs.getDen());}
        bool operator!=(const Fraction rhs) const {return !(*this == rhs);}
        bool operator>(const Fraction& rhs) const
        {
            unsigned newden = (getDen()*rhs.getDen())/gcd(getDen(),rhs.getDen()), num1 = getNum()*newden/getDen(), num2 = rhs.getNum()*newden/rhs.getDen();
            return (num1 > num2);
        }
        bool operator<(const Fraction rhs) const
        {
            return !((*this > rhs) || (*this == rhs));
        }
        bool operator<=(const Fraction rhs) const
        {
            return (*this < rhs || *this == rhs);
        }
        bool operator>=(const Fraction rhs) const
        {
            return (*this > rhs || *this == rhs);
        }
        // multiplication is needed when calculating the spray size if the ratio is acceptable
        Fraction operator*(unsigned rhs)
        {
            return Fraction (getNum()*rhs, getDen());
        }
};

// to print out the ratios
ostream& operator<<(ostream& out, const Fraction input)
{
    return out << input.getNum() << '/' << input.getDen();
}

// this function returns allowed ratios based on the rule that if a/b is a ratio of 2 ints, then a*b has to be <= 56
// to be acceptable
// to avoid repeating results, only ratios >= 1 are generated - no reciprocals here
set<Fraction> ratio_gen()
{
    set<Fraction> ratios = {};
    for (unsigned a = 1; a <= NUMTIMESDEN; a++){
        for (unsigned b = 1; b <= NUMTIMESDEN; b++){
            if (a*b <= NUMTIMESDEN && a >= b){ratios.insert(Fraction(a,b));}
        }
    }
    return ratios;
}

// this does the main calculation if the given ratio is not allowed, given a width and height of the original
vector<unsigned> ratiotofit(unsigned w, unsigned h)
{
    set<Fraction> ratios = ratio_gen();
    // this vector will contain all possible image sizes to trim to
    vector<unsigned> image_res = {};
    // for every ratio, it will find 2 width-height pairs - one with the product barely bigger than the original
    // image (ceiling), and one with the product barely smaller than that (floor)
    for (Fraction r : ratios){
        bool ceil_good = 0; // technically unneccessary, but oh well, removing this won't change much

        // since i only have ratios >= 1, i'll have to make sure not to mess up the calculations
        // because even for w < h the values in the vector will follow width >= height

        // the ceiling width and height start out on the smallest allowed size for a given ratio
        // e.g. for 3/2 it will be 3*16 / 2*16, or 48 x 32 - divisible by 256, or 16**2
        // floor width and height follow the same rule at the start
        unsigned wceil = r.getNum()*16, hceil = r.getDen()*16;
        unsigned wfloor = wceil, hfloor = hceil;
        while (true){
            // we'll try to add 16 times the numerator to the floor width and 16 times the denominator to the floor height
            // and check if the new image size is smaller than the original
            // e.g. with 3/2 we start out at 48x32. make it 48+48 x 32+32, does it fit still?
            if ((wfloor + r.getNum()*16)*(hfloor + r.getDen()*16) < (w*h)){
                // go on with the addition
                wfloor += r.getNum()*16; hfloor += r.getDen()*16;
            }
            else{
                // if adding will make the floor image size bigger than the original's
                // then make the ceiling just that

                // if the original is 156x100, for example, then you'll get 144x96 as the floor
                // and 196x128 as the ceiling for the ratio of 3/2
                // notice how 196 = 144 + 3*16 and 128 = 96 + 2*16 - just one step ahead
                wceil = wfloor + r.getNum()*16; hceil = hfloor + r.getDen()*16;
                ceil_good = 1;
            }
            if (ceil_good){
                // time to leave, append the results
                image_res.push_back(wceil);
                image_res.push_back(hceil);
                image_res.push_back(wfloor);
                image_res.push_back(hfloor);
                break;
            }
        }
    }

    // here we will decide 4 best results
    unsigned w_pos = 0, h_pos = 0, // width and height for positive difference of areas (original minus new)
    w_neg = 0, h_neg = 0, // original minus new < 0 (aka negative difference of areas)
    w_pos_bestratio = 0, h_pos_bestratio = 0, // positive area difference and best ratio (that allows for the biggest spray)
    w_neg_bestratio = 0, h_neg_bestratio = 0, // negative area diff, best ratio

    // these 4 vars are the symmetric diffs, or how many pixels don't match, for every result
    // minimizing these ensures that our new image won't differ too much from the original
    // they are calculated the same way
    D_area_p = 0xffffffff, D_area_p_bestratio = 0xffffffff, D_area_n_bestratio = 0xffffffff, D_area_n = 0xffffffff,

    // to contain the symmetric difference
    symmdiff = 0;

    // these bools contain the bulk of the comparisons. the var names say it all
    // check sizes
    bool newIsBiggerEqual, newIsSmaller;

    // 56/1, 14/1, 7/2, 7/8 and their reciprocals
    bool bestratio;

    // biggest overlap == smallest symmetric diff
    bool newOverlapBetter_p, newOverlapBetter_n, newOverlapBetter_p_bestratio, newOverlapBetter_n_bestratio;
    for (unsigned p = 0; p < image_res.size() - 1; p += 2){

        // did not forget about width and height shenanigans
        symmdiff = w*h + image_res[p]*image_res[p+1] - 2*min(w < h ? h : w,image_res[p])*min(w < h ? w : h,image_res[p+1]);
        newIsBiggerEqual = (w*h >= image_res[p]*image_res[p+1]);
        newIsSmaller = !(newIsBiggerEqual);
        newOverlapBetter_p = (D_area_p > symmdiff);
        newOverlapBetter_n = (D_area_n > symmdiff);
        if (newIsBiggerEqual && newOverlapBetter_p){
            D_area_p = symmdiff;
            w_pos = image_res[p]; h_pos = image_res[p+1];
        }
        if (newIsSmaller && newOverlapBetter_n){
            D_area_n = symmdiff;
            w_neg = image_res[p]; h_neg = image_res[p+1];
        }

        // the fraction's here just to avoid annoying usage of gcd
        Fraction rt = {image_res[p],image_res[p+1]};
        bestratio = (rt.getDen()*rt.getNum() == 56 || rt.getDen()*rt.getNum() == 14);
        // why 56 and 14? for every pair of divisors of 56 (divisors n and 56/n),
        // after doing gcd(n, 56/n) you'll find out that n/gcd * 56/n/gcd either gives 56 or 14
        // 56: 56/1 (56*1 = 56), 7/8 (7*8 = 56)
        // 14: 2/28, or 1/14 (1*14 = 14), 4/14, or 2/7 (2*7 = 14)

        newOverlapBetter_p_bestratio = (D_area_p_bestratio > symmdiff);
        newOverlapBetter_n_bestratio = (D_area_n_bestratio > symmdiff);

        if (newIsBiggerEqual && bestratio && newOverlapBetter_p_bestratio){
            D_area_p_bestratio = symmdiff;
            w_pos_bestratio = image_res[p]; h_pos_bestratio = image_res[p+1];
        }
        if (newIsSmaller && bestratio && newOverlapBetter_n_bestratio){
            D_area_n_bestratio = symmdiff;
            w_neg_bestratio = image_res[p]; h_neg_bestratio = image_res[p+1];
        }
    }
    // that's it, append
    vector<unsigned> res = {w_pos, h_pos, w_neg, h_neg, w_pos_bestratio, h_pos_bestratio, w_neg_bestratio, h_neg_bestratio};
    return res;
}

void printouttable(unsigned w, unsigned h)
{
    // calculate the 4 best sizes
    vector<unsigned> ratios = ratiotofit(w,h);
    cout << "resolution; symmetric difference; ratio; area difference\n";
    unsigned short c = 1; // to print out 4 different comments - see the switch-case
    for (unsigned ptr = 0; ptr < ratios.size()-1; ptr += 2){

        // these 2 are self-explanatory
        unsigned symmetricdiff = w*h + ratios[ptr]*ratios[ptr+1] - 2*min(w < h ? h : w,ratios[ptr])*min(w < h ? w : h,ratios[ptr+1]);
        int areadiff = (int)(ratios[ptr]*ratios[ptr+1]) - (int)(w*h);

        // since the results of ratiotofit are always ordered bigger side - smaller side, it is neccessary to check if the order needs to be flipped
        Fraction r1;
        w < h ? r1 = {ratios[ptr+1],ratios[ptr]} : r1 = {ratios[ptr],ratios[ptr+1]};
        w < h ? cout << ratios[ptr+1] << 'x' << ratios[ptr] : cout << ratios[ptr] << 'x' << ratios[ptr+1];
        cout << ' ' << symmetricdiff << ' ';
        cout << r1;
        cout << ' ' << areadiff << ' ';
        switch (c){
            case 1: {cout << "floor, biggest overlap\n"; ++c; break;}
            case 2: {cout << "ceiling, biggest overlap\n"; ++c; break;}
            case 3: {cout << "floor, biggest overlap, biggest spray size\n"; ++c; break;}
            case 4: {cout << "ceiling, biggest overlap, biggest spray size\n"; ++c; break;}
        }
    }
}

int main()
{
    unsigned width = 0, height = 0;
    cout << "Awaiting input of width...\n";
    cin >> width;
    cout << "Awaiting input of height...\n";
    cin >> height;
    Fraction trueratio;
    // for calculations it's neccessary to make sure the ratio is >= 0
    width >= height ? trueratio = {width,height} : trueratio = {height,width};
    set<Fraction> ratios = ratio_gen();
    if (ratios.find(trueratio) != ratios.end()){
        // ratio is acceptable, print out sizes
        cout << Fraction (width, height) << " is acceptable, spray sizes:\n";
        unsigned sprayside;
        for (sprayside = NUMTIMESDEN; sprayside > 0; sprayside--){
            // printing out is a bit messy, but it's self-explanatory
            Fraction sprayside2 = trueratio*sprayside;
            if (sprayside*sprayside2.getNum() <= NUMTIMESDEN && sprayside2.getDen() == 1){
                width >= height ? cout << 16*sprayside2.getNum() << 'x' << 16*sprayside << '\n' : cout << 16*sprayside << 'x' << 16*sprayside2.getNum() << '\n';
            }
        }
        cout << "You may also cut your image down to one of the sizes below\n";
        printouttable(width, height);
    }
    else{
        cout << Fraction (width, height);
        cout << " not a perfect ratio\n";
        cout << "It's recommended to cut to one of the sizes below and then resize\n";
        printouttable(width, height);
        cout << "Restart the program and input the ratio of the chosen size to get the spray size\n";
    }
}
