#ifndef MELTINGSOLIDIFICATIONCELL_H
#define MELTINGSOLIDIFICATIONCELL_H

#include "../basecell.h"
#include "../../math/myvector3d.h"
class KornerImplementation;

class MeltingSolidificationCell : public BaseCell {
public:
    MeltingSolidificationCell(KornerImplementation *ki);
    void preUpdate(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    void preUpdate2(double epsilon[], BaseCell* neighbors[], Grid *grid, Vector3i position);
    void update();
    void postUpdate(Grid *grid, Vector3i position);
    void reset(double p0, double p1);
    double getF(int index, int component);
    void setNextF(int index, double value, int component);
    void setMulticomponent(int multicomponent);
    void passivate(QXmlStreamWriter &writer);
    void activate(QXmlStreamReader &reader, Grid *grid);
    double getP(int index);
    MyVector3D getU(int index);
    double deltaP();
    double getLastDeltaP();
    double getNextF(int index, int component);
    MyVector3D getForcesVelocity(int component);
    bool isFluid();
    int getOpenCLType();
    void updateFromOpenCL(double *data);
    BaseCell* clone();
    void setType(char type);
    char getType();
    double getCellMass();
    void convertToInterface(char newType);
    double getBeamEnergy();
    void setBeamEnergy(double beamEnergy);
    double getAbsorbedEnergy();
    void setAbsorbedEnergy(double absorbedEnergy);
private:
    double f[9], nextF[9];
    double g[9], nextG[9];
    double cellMass, nextCellMass;
    double p, T;
    MyVector3D u;
    char type; // (I)nterface, (G)as, (L)iquid, (S)olid, (W)all
    double beamEnergy;
    double absorbedEnergy;
    KornerImplementation *ki;
};

#endif // MELTINGSOLIDIFICATIONCELL_H
