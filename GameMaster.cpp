#include "GameMaster.h"
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept> // std::runtime_error
#include <iostream>
#include <ctime>
#include <cmath>
#include <algorithm>
/**
 * Main Constructor for Gamemaster handles opening or creating a new gamemaster.txt file and reads the data into variables
 */
GameMaster::GameMaster()
{
    if(fileExists((fileName)))
    {
        std::ifstream gameFile(fileName);
        if(!gameFile.is_open()) throw std::runtime_error("Could not open gamemaster.txt");

        std::string line;
            double fileContents[5][3];
            int rowIndex = 0;
            double sample, mean, stdDev;
            while(gameFile >> sample >> mean >> stdDev)
            {
                fileContents[rowIndex][0] = sample;
                fileContents[rowIndex][1] = mean;
                fileContents[rowIndex][2] = stdDev;
                rowIndex++;
            }
            gameFile.close();
            //place all variables from file
            gruntSample = fileContents[0][0];
            gruntMean = fileContents[0][1];
            gruntStd = fileContents[0][2];

            archerSample = fileContents[1][0];
            archerMean= fileContents[1][1];
            archerStd = fileContents[1][2];

            goblinSample = fileContents[2][0];
            goblinMean = fileContents[2][1];
            goblinStd = fileContents[2][2];

            knightSample = fileContents[3][0];
            knightMean = fileContents[3][1];
            knightStd = fileContents[3][2];

            assassinSample = fileContents[4][0];
            assassinMean = fileContents[4][1];
            assassinStd = fileContents[4][2];
    }
    else
    {
        //Initialize blank file
        std::ofstream gameFile(fileName);
        gameFile << "0 0 0" << "\n";
        gameFile << "0 0 0" << "\n";
        gameFile << "0 0 0" << "\n";
        gameFile << "0 0 0" << "\n";
        gameFile << "0 0 0" << "\n";
        gameFile.close();
        //SET VARIABLES = MODEL VARIABLES
        gruntMean = modelGruntMean;
        gruntSample = modelGruntSample;
        gruntStd = modelGruntStd;

        archerMean = modelArcherMean;
        archerSample = modelArcherSample;
        archerStd = modelArcherStd;

        goblinMean = modelGoblinMean;
        goblinSample = modelGoblinSample;
        goblinStd = modelGoblinStd;

        knightMean = modelKnightMean;
        knightSample = modelKnightSample;
        knightStd = modelKnightStd;

        assassinMean = modelAssassinMean;
        assassinSample = modelAssassinSample;
        assassinStd = modelAssassinStd;
    }
}

/**
 * Rewrites file with new data
 */
void GameMaster::writeFile()
{
    //call to factor in new statistical calculations
    calcNewStats();
    std::ofstream gameFile(fileName);

    if(gameFile.good()){
        gameFile << gruntSample << " " << gruntMean << " " << gruntStd << "\n";
        gameFile << archerSample << " " << archerMean << " " << archerStd << "\n";
        gameFile << goblinSample << " " << goblinMean << " " << goblinStd << "\n";
        gameFile << knightSample<< " " << knightMean << " " << knightStd << "\n";
        gameFile << assassinSample << " " << assassinMean << " " << assassinStd << "\n";
        gameFile.close();
    }
    else throw std::runtime_error("Error writing to file.");
}

/**
 * Simple helper function checking for file existing
 * @param name Name of file
 * @return true if file is found, false otherwise
 */
bool GameMaster::fileExists(const std::string &name)
{
    std::ifstream f(name.c_str());
    return f.good();
}

/******************
 * ADDING METHODS *
 ******************/

void GameMaster::addGruntDamage(double amount) {
    gruntDamage.push_back(amount);
}

void GameMaster::addArcherDamage(double amount){
    archerDamage.push_back(amount);
}

void GameMaster::addGoblinDamage(double amount){
    goblinDamage.push_back(amount);
}

void GameMaster::addKnightDamage(double amount){
    knightDamage.push_back(amount);
}

void GameMaster::addAssassinDamage(double amount){
    assassinDamage.push_back(amount);
}

void GameMaster::addGruntSample(int amount) {
    gruntSample += amount;
}

void GameMaster::addArcherSample(int amount) {
    archerSample += amount;
}

void GameMaster::addGoblinSample(int amount) {
    goblinSample += amount;
}

void GameMaster::addKnightSample(int amount) {
    knightSample += amount;
}

void GameMaster::addAssassinSample(int amount) {
    assassinSample += amount;
}

/*****************
 * STATS METHODS *
 *****************/

double GameMaster::calcMean(double damage, double sample) {
    return damage/sample;
}

double GameMaster::calcMean(double sample, std::vector<double>& damagePoints) {
    double sum = 0;
    for(double damagePoint : damagePoints)
        sum += damagePoint;
    return sum/sample;
}

/**
 * Adding the mean of the old data with the new data
 * @param sample
 * @param damagePoints
 */
void GameMaster::calcNewStats(){
    double oldGruntMean, oldGruntSample, oldArcherMean, oldArcherSample, oldGoblinMean,
    oldGoblinSample, oldKnightMean, oldKnightSample, oldAssassinMean, oldAssassinSample;
    srand(time(NULL));
    double newGruntSample, newArcherSample, newGoblinSample, newKnightSample, newAssassinSample;
    double newGruntMean, newArcherMean, newGoblinMean, newKnightMean, newAssassinMean;
    //gets the old stuff

        std::ifstream gameFile(fileName);
        if(!gameFile.is_open()) throw std::runtime_error("Could not open gamemaster.txt");
        std::string line;

            double fileContents[5][3];
            int rowIndex = 0;
            double sample, mean, stdDev;
            while(gameFile >> sample >> mean >> stdDev)
            {
                fileContents[rowIndex][0] = sample;
                fileContents[rowIndex][1] = mean;
                fileContents[rowIndex][2] = stdDev;
                rowIndex++;
            }
            gameFile.close();
            //place all variables from file
            oldGruntSample = fileContents[0][0];
            oldGruntMean = fileContents[0][1];

            oldArcherSample = fileContents[1][0];
            oldArcherMean = fileContents[1][1];

            oldGoblinSample = fileContents[2][0];
            oldGoblinMean = fileContents[2][1];

            oldKnightSample = fileContents[3][0];
            oldKnightMean = fileContents[3][1];

            oldAssassinSample = fileContents[4][0];
            oldAssassinMean = fileContents[4][1];

    gameFile.close();

    std::cout
    <<"Old Grunt Sample: " << oldGruntSample << "\n"
    <<"Old Archer Sample: "<< oldArcherSample << "\n"
    <<"Old Goblin Sample: "<< oldGoblinSample << "\n"
    <<"Old Knight Sample: "<< oldKnightSample << "\n"
    <<"Old Assassin Sample: " << oldAssassinSample << "\n\n"
    <<"Old Grunt Mean: "<< oldGruntMean << "\n"
    <<"Old Archer Mean: "<<oldArcherMean << "\n"
    <<"Old Goblin Mean: "<<oldGoblinMean << "\n"
    <<"Old Knight Mean: "<<oldKnightMean << "\n"
    <<"Old Assassin Mean: "<<oldAssassinMean << "\n";

    //find the total from this game by subtracting the old from the total
    newGruntSample = gruntSample - oldGruntSample;
    newArcherSample = archerSample - oldArcherSample;
    newGoblinSample = goblinSample - oldGoblinSample;
    newKnightSample = knightSample - oldKnightSample;
    newAssassinSample = assassinSample - oldAssassinSample;

    newGruntMean = calcMean(newGruntSample, gruntDamage);
    newArcherMean = calcMean(newArcherSample, archerDamage);
    newKnightMean = calcMean(newKnightSample, knightDamage);
    newGoblinMean = calcMean(newGoblinSample, goblinDamage);
    newAssassinMean = calcMean(newAssassinSample, assassinDamage);

    std::cout
    << "New Grunt Mean: " << newGruntMean << "\n"
    << "New Archer Mean: " << newArcherMean << "\n"
    << "New Goblin Mean: " << newGoblinMean << "\n"
    << "New Knight Mean: " << newKnightMean << "\n"
    << "New Assassin Mean: " << newAssassinMean << "\n";

//    gruntMean = totalMean(oldGruntMean,newGruntMean, oldGruntSample,newGruntSample);
//    archerMean = totalMean(oldArcherMean, newArcherMean, oldArcherSample, newArcherSample);
//    goblinMean = totalMean(oldGoblinMean, newGoblinMean, oldGoblinSample, newGoblinSample);
//    knightMean = totalMean(oldKnightMean, newKnightMean, oldKnightSample, newKnightSample);
//    assassinMean = totalMean(oldAssassinMean, newAssassinMean, oldAssassinSample, newAssassinSample);

    std::cout
    << "Grunt Total Mean: " << gruntMean << "\n"
    << "Archer Total Mean: " << archerMean << "\n"
    << "Goblin Total Mean: " << goblinMean << "\n"
    << "Knight Total Mean: " << knightMean << "\n"
    << "Assassin Total Mean: " << assassinMean << "\n";
    //take old std deviation, square it, calc new variance, add both, take square root of sum
    //square old std deviation
    gruntStd = pow(gruntStd,2);
    archerStd = pow(archerStd,2);
    goblinStd = pow(goblinStd,2);
    knightStd = pow(knightStd,2);
    assassinStd = pow(assassinStd,2);

    std::cout
            << "\nGrunt Variance: " << gruntStd << "\n"
            << "Archer Variance: " << archerStd << "\n"
            << "Goblin Variance: " << goblinStd << "\n"
            << "Knight Variance: " << knightStd << "\n"
            << "Assassin Variance: " << assassinStd << "\n";


    double gruntNew, archerNew, goblinNew, knightNew, assassinNew;
    //calc new variance
    gruntNew = calcVariance(newGruntSample,gruntDamage, newGruntMean);
    archerNew = calcVariance(newArcherSample,archerDamage, newArcherMean);
    goblinNew = calcVariance(newGoblinSample,goblinDamage, newGoblinMean);
    knightNew = calcVariance(newKnightSample,knightDamage, newKnightMean);
    assassinNew = calcVariance(newAssassinSample,assassinDamage, newAssassinMean);

    std::cout
            << "Grunt New Variance: " << gruntNew << "\n"
            << "Archer New Variance: " << archerNew << "\n"
            << "Goblin New Variance: " << goblinNew << "\n"
            << "Knight New Variance: " << knightNew << "\n"
            << "Assassin New Variance: " << assassinNew << "\n";

    //add old and new variance, sqrt them to find combined std deviation
    std::vector<double> v;
    if(gruntStd != 0){
        v = calcNewVarianceAndMean(oldGruntSample, gruntStd, newGruntSample, oldGruntSample, gruntDamage, oldGruntMean);
        gruntStd = sqrt(v.at(0));
        gruntMean = v.at(1);
    }else
        gruntStd = sqrt(gruntNew);

    if(archerStd != 0){
        v = calcNewVarianceAndMean(oldArcherSample, archerStd, newArcherSample, oldArcherSample, archerDamage, oldArcherMean);
        archerStd = sqrt(v.at(0));
        archerMean = v.at(1);
    }else
        archerStd = sqrt(archerNew);

    if(goblinStd != 0){
        v = calcNewVarianceAndMean(oldGoblinSample, goblinStd, newGoblinSample, oldGoblinSample, goblinDamage, oldGoblinMean);
        goblinStd = sqrt(v.at(0));
        goblinMean = v.at(1);
    }else
        goblinStd = sqrt(goblinNew);

    if(knightStd != 0){
        v = calcNewVarianceAndMean(oldKnightSample, knightStd, newKnightSample, oldKnightSample, knightDamage, oldKnightMean);
        goblinStd = sqrt(v.at(0));
        goblinMean = v.at(1);
    }else
        knightStd = sqrt(knightNew);

    if(assassinStd != 0){
        v = calcNewVarianceAndMean(oldAssassinSample, assassinStd, newAssassinSample, oldAssassinSample, assassinDamage, oldAssassinMean);
        goblinStd = sqrt(v.at(0));
        goblinMean = v.at(1);
    }else
        assassinStd = assassinNew;

    if(__isnan(gruntStd) || std::isinf(gruntStd) || gruntStd > 10)
        gruntStd = (10 - 1) * ( (double)rand() / (double)RAND_MAX ) + 1;
    if(__isnan(archerStd) || std::isinf(archerStd) || archerStd > 10)
        archerStd = (10 - 1) * ( (double)rand() / (double)RAND_MAX ) + 1;
    if(__isnan(goblinStd) || std::isinf(goblinStd) || goblinStd > 10)
        goblinStd = (10 - 1) * ( (double)rand() / (double)RAND_MAX ) + 1;
    if(__isnan(knightStd) || std::isinf(knightStd) || knightStd > 10)
        knightStd = (10 - 1) * ( (double)rand() / (double)RAND_MAX ) + 1;
    if(__isnan(assassinStd) || std::isinf(assassinStd) || assassinStd > 10)
        assassinStd = (10 - 1) * ( (double)rand() / (double)RAND_MAX ) + 1;
}

std::vector<double> GameMaster::calcNewVarianceAndMean(double oldSample, double oldVariance, double sampleNew, double sampleOld,
                                        const std::vector<double>&data, double oldMean){
    double sigma = oldVariance;
    double meanCalc = oldMean;
    for(auto& i : data){
        sigma = ((oldSample - 1) * sigma + (i - meanSteps(meanCalc, sampleOld, i,sampleNew))*(i - meanCalc))/sampleOld;
        meanCalc = meanSteps(meanCalc, sampleOld, i,sampleNew);
        oldSample += 1;
    }
    std::vector<double> a = {sigma, meanCalc};
    return a;
}

double GameMaster::meanSteps(double oldMean, double sampleOld, double dataPoint, double sampleNew){
    return((oldMean*sampleOld + dataPoint)/sampleNew);
}


double GameMaster::totalMean(double oldMean, double newMean, double oldSample, double newSample){
    return ((oldMean*oldSample)+(newMean*newSample))/(oldSample+newSample);
}

/**
 * Calls calcVariance function and takes the sqrt of it. See calcVariance for parameters and implementation
 * @param sample see calcVariance
 * @param damagePoints see calcVariance
 * @return double containing Std Deviation
 */
double GameMaster::calcStdDev(double sample, std::vector<double>& damagePoints){
    return std::sqrt(calcVariance(sample, damagePoints));
}

double GameMaster::calcStdDev(double sample, std::vector<double>& damagePoints, double mean){
    return std::sqrt(calcVariance(sample, damagePoints, mean));
}


/**
 * Calculates Variance of a given set of data
 * @param sample Sample size of data
 * @param damagePoints vector containing damage hit point data
 * @return double containing variance
 */
double GameMaster::calcVariance(double sample, std::vector<double>& damagePoints) {
    double mean = calcMean(sample, damagePoints);
    double topSum = 0;

    for(double damage : damagePoints)
        topSum += pow((damage - mean), 2);

    return topSum/sample;
}

double GameMaster::calcVariance(double sample, std::vector<double>& damagePoints, double mean) {
    double topSum = 0;

    for(double damage : damagePoints)
        topSum += pow((damage - mean), 2);

    return topSum/sample;
}

/**
 * Method to find all z scores. Z scores sorted low to high
 * @return vector containing Z Scores for each enemy
 */
std::vector<std::pair<std::string,double>> GameMaster::findZScores() {

    std::vector<std::pair<std::string,double>> zScore = {
            {"Grunt", zScoreCalc(gruntMean,gruntStd,gruntSample)},
            {"Archer",zScoreCalc(archerMean,archerStd,archerSample)},
            {"Goblin",zScoreCalc(goblinMean,goblinStd,goblinSample)},
            {"Knight",zScoreCalc(knightMean,knightStd,knightSample)},
            {"Assassin",zScoreCalc(assassinMean,assassinStd,assassinSample)}
    };
    //sort the zScores
    std::sort(zScore.begin(), zScore.end(), [](auto &left, auto &right) {
        return left.second < right.second;
    });
    return zScore;
}

double GameMaster::zScoreCalc(double mean, double stdDev, double sampleSize) {
    return (sampleSize - mean)/stdDev;
}