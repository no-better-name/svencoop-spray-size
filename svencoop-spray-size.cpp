#include <iostream>
#include <set>
#include <vector>
#include <cmath>

using namespace std;

unsigned gcd(unsigned a, unsigned b) {return (b == 0) ? a : gcd(b, a % b);}

class Fraction
{
        unsigned num, den;
    public:
        Fraction (unsigned n, unsigned d){
            if (d == 0){num = 0; den = 1;}
            else{num = n / gcd(n,d);den = d / gcd(n,d);}
        }
        Fraction (){
            num = 1; den = 1;
        }
        unsigned getNum() const {return num;}
        unsigned getDen() const {return den;}
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
        Fraction operator+(Fraction rhs)
        {
            unsigned newden = getDen()*rhs.getDen()/gcd(getDen(),rhs.getDen()), newnum = (getNum() * newden / getDen()) + (rhs.getNum() * newden / rhs.getDen());
            return Fraction (newnum, newden);
        }
        Fraction operator+(unsigned rhs)
        {
            unsigned newnum = getNum() + rhs*getDen();
            return Fraction (newnum, getDen());
        }
        Fraction operator-(unsigned rhs)
        {
            unsigned newnum = getNum() - rhs*getDen();
            return Fraction (newnum, getDen());
        }
        Fraction operator-(Fraction rhs)
        {
            unsigned newden = getDen()*rhs.getDen()/gcd(getDen(),rhs.getDen()), newnum = (getNum() * newden / getDen()) - (rhs.getNum() * newden / rhs.getDen());
            return Fraction (newnum, newden);
        }
        Fraction operator*(Fraction rhs)
        {
            return Fraction (getNum()*rhs.getNum(), getDen()*rhs.getDen());
        }
        Fraction operator*(unsigned rhs)
        {
            return Fraction (getNum()*rhs, getDen());
        }
        Fraction operator/(unsigned rhs)
        {
            return Fraction (getNum(), getDen()*rhs);
        }
        Fraction operator/(Fraction rhs)
        {
            return Fraction (getNum()*rhs.getDen(), getDen()*rhs.getNum());
        }
};

ostream& operator<<(ostream& out, const Fraction input)
{
    return out << input.getNum() << '/' << input.getDen();
}

set<Fraction> ratio_gen()
{
    set<Fraction> ratios = {};
    for (unsigned a = 1; a <= 56; a++){
        for (unsigned b = 1; b <= 56; b++){
            if (a*b <= 56 && a >= b){ratios.insert(Fraction(a,b));}
        }
    }
    return ratios;
}

vector<unsigned> ratiotofit(unsigned w, unsigned h)
{
    set<Fraction> ratios = ratio_gen();
    vector<unsigned> image_res = {};
    for (Fraction r : ratios){
        bool ceil_good = 0;
        unsigned wceil = (w > h) ? r.getNum()*16 : r.getDen()*16, hceil = (w > h) ? r.getDen()*16 : r.getNum()*16;
        unsigned wfloor = wceil, hfloor = hceil;
        while (true){
            if (w > h){
                if ((wfloor + r.getNum()*16)*(hfloor + r.getDen()*16) < (w*h)){
                    wfloor += r.getNum()*16; hfloor += r.getDen()*16;
                }
                else{
                    wceil = wfloor + r.getNum()*16; hceil = hfloor + r.getDen()*16;
                    ceil_good = 1;
                }
                if (ceil_good){
                    image_res.push_back(wceil);
                    image_res.push_back(hceil);
                    image_res.push_back(wfloor);
                    image_res.push_back(hfloor);
                    break;
                }
            }
            else{
                if ((wfloor + r.getDen()*16)*(hfloor + r.getNum()*16) < (w*h)){
                    wfloor += r.getDen()*16; hfloor += r.getNum()*16;
                }
                else{
                    wceil = wfloor + r.getDen()*16; hceil = hfloor + r.getNum()*16;
                    ceil_good = 1;
                }
                if (ceil_good){
                    image_res.push_back(wceil);
                    image_res.push_back(hceil);
                    image_res.push_back(wfloor);
                    image_res.push_back(hfloor);
                    break;
                }
            }
        }
    }
    unsigned w_pos = 0, h_pos = 0,
    w_neg = 0, h_neg = 0,
    w_pos_76 = 0, h_pos_76 = 0,
    w_neg_76 = 0, h_neg_76 = 0,
    D_area_p = 0 - 1, D_area_p_76 = 0 - 1, D_area_n_76 = 0 - 1, D_area_n = 0 - 1;
    bool biggerandmostoverlap, smallerandmostoverlap, bigger76andmostoverlap, smaller76andmostoverlap;
    for (unsigned p = 0; p < image_res.size() - 1; p += 2){
        biggerandmostoverlap = (w*h >= image_res[p]*image_res[p+1] && D_area_p > w*h + image_res[p]*image_res[p+1] - 2*min(w,image_res[p])*min(h,image_res[p+1]));
        smallerandmostoverlap = (image_res[p]*image_res[p+1] > w*h && D_area_n > w*h + image_res[p]*image_res[p+1] - 2*min(w,image_res[p])*min(h,image_res[p+1]));
        if (biggerandmostoverlap){
            D_area_p = w*h + image_res[p]*image_res[p+1] - 2*min(w,image_res[p])*min(h,image_res[p+1]);
            w_pos = image_res[p]; h_pos = image_res[p+1];
        }
        if (smallerandmostoverlap){
            D_area_n = w*h + image_res[p]*image_res[p+1] - 2*min(w,image_res[p])*min(h,image_res[p+1]);
            w_neg = image_res[p]; h_neg = image_res[p+1];
        }
        Fraction rt = {image_res[p],image_res[p+1]};
        bigger76andmostoverlap = ((rt.getDen()*rt.getNum() == 56 || rt.getDen()*rt.getNum() == 14) && w*h >= image_res[p]*image_res[p+1] && D_area_p_76 > w*h + image_res[p]*image_res[p+1] - 2*min(w,image_res[p])*min(h,image_res[p+1]));
        smaller76andmostoverlap = ((rt.getDen()*rt.getNum() == 56 || rt.getDen()*rt.getNum() == 14) && image_res[p]*image_res[p+1] > w*h && D_area_n_76 > w*h + image_res[p]*image_res[p+1] - 2*min(w,image_res[p])*min(h,image_res[p+1]));
        if (bigger76andmostoverlap){
            D_area_p_76 = w*h + image_res[p]*image_res[p+1] - 2*min(w,image_res[p])*min(h,image_res[p+1]);
            w_pos_76 = image_res[p]; h_pos_76 = image_res[p+1];
        }
        if (smaller76andmostoverlap){
            D_area_n_76 = w*h + image_res[p]*image_res[p+1] - 2*min(w,image_res[p])*min(h,image_res[p+1]);
            w_neg_76 = image_res[p]; h_neg_76 = image_res[p+1];
        }
    }
    vector<unsigned> res = {w_pos, h_pos, w_neg, h_neg, w_pos_76, h_pos_76, w_neg_76, h_neg_76};
    return res;
}

void printouttable(unsigned w, unsigned h)
{
    vector<unsigned> ratios = ratiotofit(w,h);
    cout << "resolution; symmetric difference; ratio; area difference\n";
    unsigned short c = 1;
    for (unsigned ptr = 0; ptr < ratios.size()-1; ptr += 2){
        Fraction r1 (ratios[ptr],ratios[ptr+1]);
        cout << ratios[ptr] << 'x' << ratios[ptr+1] << ' ' << w*h + ratios[ptr]*ratios[ptr+1] - 2*min(w,ratios[ptr])*min(h,ratios[ptr+1]) << ' ';
        cout << r1;
        cout << ' ' << (long)(ratios[ptr]*ratios[ptr+1]) - (long)(w*h) << ' ';
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
    width >= height ? trueratio = {width,height} : trueratio = {height,width};
    set<Fraction> ratios = ratio_gen();
    if (ratios.find(trueratio) != ratios.end()){
        cout << Fraction (width, height) << " is acceptable, spray sizes:\n";
        unsigned sprayside;
        for (sprayside = 56; sprayside > 0; sprayside--){
            Fraction sprayside2 = trueratio*sprayside;
            if (sprayside*sprayside2.getNum() <= 56 && sprayside2.getDen() == 1){
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
