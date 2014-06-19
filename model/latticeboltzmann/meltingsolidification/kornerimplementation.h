#ifndef KORNERIMPLEMENTATION_H
#define KORNERIMPLEMENTATION_H

class Grid;

class KornerImplementation {
public:
    KornerImplementation(Grid *grid);
    void configure(double power, double standardDeviation, int startX, double velocity, int timesteps, double absorptionCoefficient);
    void configureThermal(double solidTau, double liquidTau, double gasTau);
    void configureEnergy(double solidEnergy, double liquidEnergy);
    void preUpdate();
    double getSolidTau();
    double getLiquidTau();
    double getGasTau();
    double getSolidEnergy();
    double getLiquidEnergy();
private:
    Grid *grid;
    double power, standardDeviation, velocity;
    int startX, timesteps;
    double currentX;
    double absorptionCoefficient;
    double solidTau, liquidTau, gasTau;
    double solidEnergy, liquidEnergy;
};

#endif // KORNERIMPLEMENTATION_H
