import shared3p;
import stdlib;

domain pd_shared3p shared3p;
public uint NUM_PLAYERS=4;
public uint32 INT_MAX=2147483646;

//// Auction for 1 resources with 1 attribute
void main() {

    pd_shared3p uint32 globalMinIndex = 0;
    pd_shared3p uint32 globalMinCost = INT_MAX;
    pd_shared3p uint32 secondPrice = INT_MAX;
    pd_shared3p uint32 curIndex = 2; // 0=none, 1=alice, 2=first bob
    pd_shared3p uint32 one = 1;

    pd_shared3p uint32 [[1]] db (NUM_PLAYERS);

    // populate dummy bids
    for (uint32 i=0; i<(uint32)shape(db)[0]; i++) {
        db[(uint64)i] = i+4;
    }

    for (uint i=0; i<(uint)shape(db)[0]; i++) {
        pd_shared3p bool isMin = db[i] < globalMinCost;
        secondPrice = ((uint32)isMin * globalMinCost) + ((uint32)!isMin * secondPrice);
        globalMinCost = ((uint32)isMin * db[i]) + ((uint32)!isMin * globalMinCost);
        globalMinIndex = ((uint32)isMin * curIndex) + ((uint32)!isMin * globalMinIndex);

        pd_shared3p bool isSecondMin = !isMin & (db[i] < secondPrice);
        secondPrice = ((uint32)isSecondMin * db[i]) + ((uint32)!isSecondMin * secondPrice);
        curIndex = curIndex + one;
    }

    print("Auction result: ");
    print(declassify(globalMinIndex));
    print(declassify(secondPrice));
}
