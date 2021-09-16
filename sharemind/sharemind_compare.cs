import shared3p;
import stdlib;

domain pd_shared3p shared3p;

// no enums :(
// structs cant be private or members of arrays :(
// SecreC is a KLUDGE

void main() {
    pd_shared3p uint32 a = 1;
    pd_shared3p uint32 b = 1;
    pd_shared3p bool res = a>b;

    print("Auction result: ");
    print(declassify(res));
}
