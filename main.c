#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define NUM_SPECIALTIES 4
#define NUM_WARDS       4
#define BEDS_PER_WARD   20
#define MAX_PATIENTS    100


static const int   specialtyIDs[NUM_SPECIALTIES]     = {1, 2, 3, 4};
static const char *specialtyNames[NUM_SPECIALTIES]   = {
    "General Practice (OPD)",
    "Paediatrics",
    "Cardiology",
    "Neurology"
};
static const float baseFees[NUM_SPECIALTIES]         = {1500.0f, 2500.0f, 4500.0f, 5000.0f};
static const int   consultTimes[NUM_SPECIALTIES]     = {15, 20, 30, 30};
static const int   dailyCaps[NUM_SPECIALTIES]        = {30, 20, 12, 10};

static const int   wardIDs[NUM_WARDS]                = {1, 2, 3, 4};
static const char *wardNames[NUM_WARDS]              = {
    "General Ward",
    "Pediatric Ward",
    "ICU",
    "Neurology Ward"
};
static const float wardDailyRates[NUM_WARDS]         = {
    8000.0f, 10000.0f, 25000.0f, 18000.0f
};


static int bedOccupancy[NUM_WARDS][BEDS_PER_WARD];
static int specialtyQueue[NUM_SPECIALTIES];


static int   patientIDs[MAX_PATIENTS];
static char  patientNames[MAX_PATIENTS][50];
static int   patientAges[MAX_PATIENTS];
static int   patientUrgency[MAX_PATIENTS];
static int   patientSpecialty[MAX_PATIENTS];
static int   patientAdmitted[MAX_PATIENTS];
static int   patientWard[MAX_PATIENTS];
static int   patientBed[MAX_PATIENTS];
static int   patientDays[MAX_PATIENTS];
static float patientBaseFee[MAX_PATIENTS];
static float patientSurcharge[MAX_PATIENTS];
static float patientWardCost[MAX_PATIENTS];
static float patientGross[MAX_PATIENTS];
static float patientDiscount[MAX_PATIENTS];
static float patientFinal[MAX_PATIENTS];
static float patientWaitTime[MAX_PATIENTS];

static int patientCount = 0;


void  printSeparator(void);
void  printHeader(const char *title);
void  initializeSystem(void);
void  showMainMenu(void);


int   registerPatient(void);
void  displayAllPatients(void);
void  sortByPriority(int order[]);
void  displayPriorityOrder(void);
void  generateReports(void);
