import shared3p;
import stdlib;

domain pd_shared3p shared3p;

// no enums :(
// structs cant be private or members of arrays :(
// SecreC is a KLUDGE

public uint ATTRIBUTE_UNIT=0;
public uint32 UNIT_CPU=1000;
public uint32 UNIT_RAM=2000;
public uint32 UNIT_GPU=3000;

public uint ATTRIBUTE_PRICE=1;

public uint ATTRIBUTE_QUANT=2;


template<domain D : shared3p>
D uint32 auction(D uint32 [[2]] lookingFor, D uint32 [[3]] db) {

    D uint32 globalMinCost = UINT32_MAX;
    D uint32 minIndex = UINT32_MAX;


    // For each available resource
    for (uint dbrec=0; dbrec<(uint)shape(db)[0]; dbrec++) {

        D uint32 totalCost = 0;
        D uint32 numAttributesSatisfied = 0;

        // For each attribute of the available resource11
        for (uint dbatt=0; dbatt<(uint)shape(db)[1]; dbatt++) {

            // For each attribute requested
            for (uint lfatt=0; lfatt<(uint)shape(lookingFor)[0]; lfatt++) {
                D bool sameUnit = db[dbrec, dbatt, ATTRIBUTE_UNIT] == lookingFor[lfatt, ATTRIBUTE_UNIT];
                D bool enoughAvailable = db[dbrec, dbatt, ATTRIBUTE_QUANT] <= lookingFor[lfatt, ATTRIBUTE_QUANT];

                //print("same unit: ");
                //print(declassify(sameUnit));
                //print("enough available: ");
                //print(declassify(enoughAvailable));

                D bool kosher = sameUnit & enoughAvailable;
                D uint32 thisCost = lookingFor[lfatt, ATTRIBUTE_QUANT] * db[dbrec, dbatt, ATTRIBUTE_PRICE];
                //print("this cost: ");
                //print(declassify(thisCost));

                totalCost = totalCost + ((uint32)kosher * thisCost); // if(kosher) { totalCost += thisCost }
                numAttributesSatisfied = numAttributesSatisfied + (uint32)kosher; // if(kosher) { numAttributesSatisfied += 1 }
                //print("total cost: ");
                //print(declassify(totalCost));
                //print("numAttributesSatisfied: ");
                //print(declassify(numAttributesSatisfied));
            }
        }

        D bool isMin = totalCost < globalMinCost;
        D bool allAttributesSatisfied = numAttributesSatisfied >= (uint32)shape(lookingFor)[0];
        D bool koshertwo = isMin & allAttributesSatisfied;
        globalMinCost = (uint32)koshertwo * (totalCost - globalMinCost) + globalMinCost; // if(koshertwo) { globalMinCost = totalCost }
        //print("globalMinCost: ");
        //print(declassify(globalMinCost));
    }

    return globalMinCost;
}



//// Auction for 2 resources each with 3 attributes
//void main() {
//    pd_shared3p uint32 [[3]] bobsdb (2,3,3);
//
//    //// Resource 0
//    // Attribute 0
//    bobsdb[0, 0, ATTRIBUTE_UNIT] = UNIT_CPU;
//    bobsdb[0, 0, ATTRIBUTE_PRICE] = 1;
//    bobsdb[0, 0, ATTRIBUTE_QUANT] = 1;
//
//    // Attribute 1
//    bobsdb[0, 1, ATTRIBUTE_UNIT] = UNIT_RAM;
//    bobsdb[0, 1, ATTRIBUTE_PRICE] = 2;
//    bobsdb[0, 1, ATTRIBUTE_QUANT] = 5;
//
//    // Attribute 2
//    bobsdb[0, 2, ATTRIBUTE_UNIT] = UNIT_GPU;
//    bobsdb[0, 2, ATTRIBUTE_PRICE] = 3;
//    bobsdb[0, 2, ATTRIBUTE_QUANT] = 10;
//
//    //// Resource 1
//    // Attribute 0
//    bobsdb[1, 0, ATTRIBUTE_UNIT] = UNIT_CPU;
//    bobsdb[1, 0, ATTRIBUTE_PRICE] = 3;
//    bobsdb[1, 0, ATTRIBUTE_QUANT] = 1;
//
//    // Attribute 1
//    bobsdb[1, 1, ATTRIBUTE_UNIT] = UNIT_RAM;
//    bobsdb[1, 1, ATTRIBUTE_PRICE] = 2;
//    bobsdb[1, 1, ATTRIBUTE_QUANT] = 5;
//
//    // Attribute 2
//    bobsdb[1, 2, ATTRIBUTE_UNIT] = UNIT_GPU;
//    bobsdb[1, 2, ATTRIBUTE_PRICE] = 1;
//    bobsdb[1, 2, ATTRIBUTE_QUANT] = 10;
//
//
//    pd_shared3p uint32 [[2]] alicewants (2,3);
//    // Attribute 0
//    alicewants[0, ATTRIBUTE_UNIT] = UNIT_CPU;
//    alicewants[0, ATTRIBUTE_QUANT] = 1;
//
//    // Attribute 1
//    alicewants[1, ATTRIBUTE_UNIT] = UNIT_RAM;
//    alicewants[1, ATTRIBUTE_QUANT] = 1;
//
//    pd_shared3p uint32 res = auction(alicewants, bobsdb);
//    print("Auction result: ");
//    print(declassify(res));
//}



//// Auction for 1 resources with 1 attribute
void main() {
    pd_shared3p uint32 [[3]] bobsdb (1,1,3);

    //// Resource 0
    // Attribute 0
    bobsdb[0, 0, ATTRIBUTE_UNIT] = UNIT_CPU;
    bobsdb[0, 0, ATTRIBUTE_PRICE] = 1;
    bobsdb[0, 0, ATTRIBUTE_QUANT] = 1;

    pd_shared3p uint32 [[2]] alicewants (2,3);
    // Attribute 0
    alicewants[0, ATTRIBUTE_UNIT] = UNIT_CPU;
    alicewants[0, ATTRIBUTE_QUANT] = 1;

    pd_shared3p uint32 res = auction(alicewants, bobsdb);
    print("Auction result: ");
    print(declassify(res));
}
