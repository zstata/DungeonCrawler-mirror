#ifndef GAMEMASTER_GAMEMASTER_H
#define GAMEMASTER_GAMEMASTER_H
#include <string>
#include <vector>
 /**
  * TXT FILE SETUP:  sample size, mean, std deviation
  * Line 1 Grunt
  * Line 2 Archer
  * Line 3 Goblin
  * Line 4 Knight
  * Line 5 Assassin
  */
class GameMaster {

private:
    double gruntSample{}, gruntMean{}, gruntStd{};
    double archerSample{}, archerMean{}, archerStd{};
    double goblinSample{}, goblinMean{}, goblinStd{};
    double knightSample{}, knightMean{}, knightStd{};
    double assassinSample{}, assassinMean{}, assassinStd{};
    double gruntZ{}, archerZ{}, goblinZ{},knightZ{}, assassinZ{};
    std::vector<double> gruntDamage{}, archerDamage{}, goblinDamage{}, knightDamage{}, assassinDamage{};
    const std::string fileName = "gamemaster.txt";
    static bool fileExists (const std::string& name);
    static double zScoreCalc(double mean, double stdDev, double sampleSize);
    double totalMean(double oldMean, double newMean, double oldSample, double newSample);
    std::vector<double> calcNewVarianceAndMean(double oldSample, double oldVariance, double sampleNew, double sampleOld,
                                  const std::vector<double> &data, double oldMean);

public:
    GameMaster();
    void addGruntDamage(double amount);
    void addArcherDamage(double amount);
    void addGoblinDamage(double amount);
    void addKnightDamage(double amount);
    void addAssassinDamage(double amount);
    void addGruntSample(int amount);
    void addArcherSample(int amount);
    void addGoblinSample(int amount);
    void addKnightSample(int amount);
    void addAssassinSample(int amount);
    double calcStdDev(double sample, std::vector<double>& damagePoints);
    double calcVariance(double sample, std::vector<double>& damagePoints);
    double calcMean(double damage, double sample);
    double calcMean(double sample, std::vector<double>&damagePoints);
    std::vector<std::pair<std::string, double>> findZScores();
    void writeFile();
    void calcNewStats();
    double calcVariance(double sample, std::vector<double> &damagePoints, double mean);
    double calcStdDev(double sample, std::vector<double> &damagePoints, double mean);

    /******HARD CODED MODEL STANDARD VALUES******
     * Enemy Spawn Percentages                  *
     *  Grunt 24%                               *
     *  Archers 12%                             *
     *  Goblins 40% (account for squads)        *
     *  Knights 8%                              *
     *  Assassins 12%                           *
     ********************************************/
	double modelGruntMean = 20,    modelGruntSample = 6,    modelGruntStd = 1.5,    modelGruntThreshold = 1.5;
    double modelArcherMean = 12,   modelArcherSample = 4,   modelArcherStd = 2.5,   modelArcherThreshold = 2.5;
    double modelGoblinMean = 10,   modelGoblinSample = 10,  modelGoblinStd = 2.5,   modelGoblinThreshold = 4.5;
    double modelKnightMean = 25,   modelKnightSample = 2,   modelKnightStd = 4.2,   modelKnightThreshold = 9.5;
    double modelAssassinMean = 15, modelAssassinSample = 4, modelAssassinStd = 2.4, modelAssassinThreshold = 3;
	 /*
    double modelGruntMean = 20,    modelGruntSample = 6,    modelGruntStd = 1.5,    modelGruntThreshold = 0;
    double modelArcherMean = 12,   modelArcherSample = 4,   modelArcherStd = 2.5,   modelArcherThreshold = 0;
    double modelGoblinMean = 10,   modelGoblinSample = 10,  modelGoblinStd = 2.5,   modelGoblinThreshold = 0;
    double modelKnightMean = 25,   modelKnightSample = 2,   modelKnightStd = 4.2,   modelKnightThreshold = 0;
    double modelAssassinMean = 15, modelAssassinSample = 4, modelAssassinStd = 2.4, modelAssassinThreshold = 0;
	*/
    double meanSteps(double oldMean, double sampleOld, double dataPoint, double sampleNew);


};

#endif //GAMEMASTER_GAMEMASTER_H