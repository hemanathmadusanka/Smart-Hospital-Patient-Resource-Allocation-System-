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

float calcWaitingTime(int specialtyID);
float calcEmergencySurcharge(int urgency, float baseFee);
float calcWardCost(int days, int wardID);
float calcGross(float baseFee, float surcharge, float wardCost);
float calcDiscount(float gross, int age);
float calcFinal(float gross, float discount);


int   findFreeBed(int wardID);
void  releaseBed(int wardID, int bedIndex);
float wardOccupancyPercent(int wardID);


void  saveBedStatus(void);
void  loadBedStatus(void);
void  appendPatientRecord(int idx);


void printSeparator(void) {
    printf("=================================================================\n");
}

void printHeader(const char *title) {
    printSeparator();
    printf("  %s\n", title);
    printSeparator();
}

void initializeSystem(void) {
    memset(bedOccupancy, 0, sizeof(bedOccupancy));
    memset(specialtyQueue, 0, sizeof(specialtyQueue));
    patientCount = 0;
}


float calcWaitingTime(int specialtyID) {
    int idx = specialtyID - 1;
    return (float)specialtyQueue[idx] * (float)consultTimes[idx];
}

float calcEmergencySurcharge(int urgency, float baseFee) {
    if (urgency == 3) return baseFee * 0.50f;
    if (urgency == 2) return baseFee * 0.25f;
    return 0.0f;
}

float calcWardCost(int days, int wardID) {
    if (wardID < 1 || wardID > NUM_WARDS) return 0.0f;
    return (float)days * wardDailyRates[wardID - 1];
}

float calcGross(float baseFee, float surcharge, float wardCost) {
    return baseFee + surcharge + wardCost;
}

float calcDiscount(float gross, int age) {
    if (age < 5 || age > 65) return gross * 0.15f;
    return 0.0f;
}

float calcFinal(float gross, float discount) {
    return gross - discount;
}


int findFreeBed(int wardID) {
    if (wardID < 1 || wardID > NUM_WARDS) return -1;
    int w = wardID - 1;
    for (int b = 0; b < BEDS_PER_WARD; b++)
        if (bedOccupancy[w][b] == 0) return b;
    return -1;
}

void releaseBed(int wardID, int bedIndex) {
    if (wardID < 1 || wardID > NUM_WARDS) return;
    if (bedIndex < 0 || bedIndex >= BEDS_PER_WARD) return;
    bedOccupancy[wardID - 1][bedIndex] = 0;
}

float wardOccupancyPercent(int wardID) {
    if (wardID < 1 || wardID > NUM_WARDS) return 0.0f;
    int w = wardID - 1;
    int occupied = 0;
    for (int b = 0; b < BEDS_PER_WARD; b++)
        if (bedOccupancy[w][b] == 1) occupied++;
    return (occupied * 100.0f) / BEDS_PER_WARD;
}


int registerPatient(void) {
    if (patientCount >= MAX_PATIENTS) {
        printf(">> System full. Cannot register more patients.\n");
        return -1;
    }

    int i = patientCount;
    printHeader("NEW PATIENT REGISTRATION");


    patientIDs[i] = 1001 + i;
    printf("Auto-assigned Patient ID: PAT-%d\n", patientIDs[i]);


    printf("Enter Patient Name           : ");
    scanf(" %[^\n]", patientNames[i]);


    printf("Enter Patient Age            : ");
    scanf("%d", &patientAges[i]);


    do {
        printf("Emergency Level (1=Normal,2=Urgent,3=Critical): ");
        scanf("%d", &patientUrgency[i]);
        if (patientUrgency[i] < 1 || patientUrgency[i] > 3)
            printf("  Invalid! Try again.\n");
    } while (patientUrgency[i] < 1 || patientUrgency[i] > 3);


    printf("\nAvailable Specialties:\n");
    for (int s = 0; s < NUM_SPECIALTIES; s++)
        printf("  %d. %-25s Fee: LKR %.2f | %d mins | Cap: %d/day\n",
               specialtyIDs[s], specialtyNames[s], baseFees[s],
               consultTimes[s], dailyCaps[s]);

    do {
        printf("Select Specialty ID (1-4): ");
        scanf("%d", &patientSpecialty[i]);
        if (patientSpecialty[i] < 1 || patientSpecialty[i] > 4)
            printf("  Invalid! Try again.\n");
    } while (patientSpecialty[i] < 1 || patientSpecialty[i] > 4);

    int spIdx = patientSpecialty[i] - 1;

    if (specialtyQueue[spIdx] >= dailyCaps[spIdx])
        printf(">> WARNING: Daily cap reached for %s.\n", specialtyNames[spIdx]);

    patientBaseFee[i]  = baseFees[spIdx];
    patientWaitTime[i] = calcWaitingTime(patientSpecialty[i]);
    specialtyQueue[spIdx]++;


    printf("\nAdmit to Ward? (1=Yes, 0=No): ");
    scanf("%d", &patientAdmitted[i]);

    patientWard[i] = 0;
    patientBed[i]  = -1;
    patientDays[i] = 0;

    if (patientAdmitted[i] == 1) {
        printf("Available Wards:\n");
        for (int w = 0; w < NUM_WARDS; w++)
            printf("  %d. %-18s Daily Rate: LKR %.2f | Occupancy: %.1f%%\n",
                   wardIDs[w], wardNames[w], wardDailyRates[w],
                   wardOccupancyPercent(wardIDs[w]));

        do {
            printf("Select Ward ID (1-4): ");
            scanf("%d", &patientWard[i]);
            if (patientWard[i] < 1 || patientWard[i] > 4)
                printf("  Invalid! Try again.\n");
        } while (patientWard[i] < 1 || patientWard[i] > 4);

        int bed = findFreeBed(patientWard[i]);
        if (bed < 0) {
            printf(">> No free beds in %s! Admission cancelled.\n",
                   wardNames[patientWard[i] - 1]);
            patientAdmitted[i] = 0;
            patientWard[i]     = 0;
        } else {
            patientBed[i] = bed;
            bedOccupancy[patientWard[i] - 1][bed] = 1;

            printf("Enter Days Admitted: ");
            scanf("%d", &patientDays[i]);
            if (patientDays[i] < 0) patientDays[i] = 0;
        }
    }


    patientSurcharge[i] = calcEmergencySurcharge(patientUrgency[i], patientBaseFee[i]);
    patientWardCost[i]  = calcWardCost(patientDays[i], patientWard[i]);
    patientGross[i]     = calcGross(patientBaseFee[i], patientSurcharge[i], patientWardCost[i]);
    patientDiscount[i]  = calcDiscount(patientGross[i], patientAges[i]);
    patientFinal[i]     = calcFinal(patientGross[i], patientDiscount[i]);

    patientCount++;


    printSeparator();
    printf("        SMART HOSPITAL ADMISSION & BILL\n");
    printSeparator();
    printf("Patient ID           : PAT-%d\n", patientIDs[i]);
    printf("Patient Name         : %s\n", patientNames[i]);

    if (patientAges[i] < 5 || patientAges[i] > 65)
        printf("Age                  : %d Years (15%% Subsidy Eligible)\n", patientAges[i]);
    else
        printf("Age                  : %d Years\n", patientAges[i]);

    printf("Specialty            : %s\n", specialtyNames[spIdx]);

    if (patientAdmitted[i])
        printf("Assigned Ward        : %s (Bed #%02d)\n",
               wardNames[patientWard[i] - 1], patientBed[i] + 1);
    else
        printf("Assigned Ward        : Outpatient / OPD\n");

    const char *urgStr = (patientUrgency[i] == 3) ? "Level 3 (Critical)" :
                         (patientUrgency[i] == 2) ? "Level 2 (Urgent)"   :
printf("Urgency Level        : %s\n", urgStr);
    printf("Base Consultation Fee: LKR %.2f\n", patientBaseFee[i]);
    printf("Emergency Surcharge  : LKR %.2f\n", patientSurcharge[i]);
    printf("Ward Stay Cost (%d Days): LKR %.2f\n", patientDays[i], patientWardCost[i]);
    printf("Gross Total Bill     : LKR %.2f\n", patientGross[i]);
    printf("Age Subsidy Discount : LKR -%.2f\n", patientDiscount[i]);
    printf("Final Payable Amount : LKR %.2f\n", patientFinal[i]);                                                    "Level 1 (Normal)";
if (patientUrgency[i] == 3 && patientWaitTime[i] == 0.0f)
        printf("Estimated Waiting Time: 0.00 mins (Immediate Attention)\n");
    else
        printf("Estimated Waiting Time: %.2f mins\n", patientWaitTime[i]);

    printSeparator();


    appendPatientRecord(i);
    saveBedStatus();

    return i;
    }


void displayAllPatients(void) {
    if (patientCount == 0) {
        printf(">> No patients registered yet.\n");
        return;
    }
    printHeader("ALL REGISTERED PATIENTS");
  printf("%-10s %-20s %-5s %-10s %-22s %-10s\n",
           "PatID", "Name", "Age", "Urgency", "Specialty", "Final(LKR)");
    printSeparator();
    for (int i = 0; i < patientCount; i++) {
        const char *urg = (patientUrgency[i] == 3) ? "Critical" :
                          (patientUrgency[i] == 2) ? "Urgent"   : "Normal";
        printf("PAT-%-6d %-20s %-5d %-10s %-22s %-10.2f\n",
               patientIDs[i], patientNames[i], patientAges[i], urg,
               specialtyNames[patientSpecialty[i] - 1], patientFinal[i]);
    }
    printSeparator();
}
void sortByPriority(int order[]) {
    for (int i = 0; i < patientCount; i++) order[i] = i;


    for (int i = 0; i < patientCount - 1; i++) {
        int best = i;
        for (int j = i + 1; j < patientCount; j++) {
            int uj = patientUrgency[order[j]];
            int ub = patientUrgency[order[best]];
            if (uj > ub) {
                best = j;
            } else if (uj == ub && order[j] < order[best]) {
                best = j;
            }
        }
        if (best != i) {
            int tmp    = order[i];
            order[i]   = order[best];
            order[best] = tmp;
        }
    }
}
void displayPriorityOrder(void) {
    if (patientCount == 0) {
        printf(">> No patients registered yet.\n");
        return;
    }
    int order[MAX_PATIENTS];
    sortByPriority(order);

    printHeader("TRIAGE PRIORITY QUEUE (Critical -> Urgent -> Normal)");
    printf("%-5s %-10s %-20s %-10s %-22s\n",
           "Rank", "PatID", "Name", "Urgency", "Specialty");
    printSeparator();
    for (int k = 0; k < patientCount; k++) {
        int i = order[k];
        const char *urg = (patientUrgency[i] == 3) ? "Critical" :
                          (patientUrgency[i] == 2) ? "Urgent"   : "Normal";
        printf("%-5d PAT-%-6d %-20s %-10s %-22s\n",
               k + 1, patientIDs[i], patientNames[i], urg,
               specialtyNames[patientSpecialty[i] - 1]);
    }
    printSeparator();
}

void generateReports(void) {
    printHeader("PERFORMANCE REPORTS & ANALYTICS");

    if (patientCount == 0) {
        printf(">> No data available. Register patients first.\n");
        return;
    }

    int c1 = 0, c2 = 0, c3 = 0;
    float totalRevenue  = 0.0f;
    float totalDiscount = 0.0f;
    float highestBill   = 0.0f;
    char  highestName[50] = "";

    for (int i = 0; i < patientCount; i++) {
        if (patientUrgency[i] == 1) c1++;
        else if (patientUrgency[i] == 2) c2++;
        else c3++;

        totalRevenue  += patientFinal[i];
        totalDiscount += patientDiscount[i];

        if (patientFinal[i] > highestBill) {
            highestBill = patientFinal[i];
            strcpy(highestName, patientNames[i]);
        }
    }
  printf("\n[1] PATIENT CATEGORIES\n");
    printf("    Total Patients Registered : %d\n", patientCount);
    printf("    Normal   (Level 1)        : %d\n", c1);
    printf("    Urgent   (Level 2)        : %d\n", c2);
    printf("    Critical (Level 3)        : %d\n", c3);
    printf("\n[2] FINANCIAL SUMMARY\n");
    printf("    Total Revenue Earned      : LKR %.2f\n", totalRevenue);
    printf("    Total Discounts Granted   : LKR %.2f\n", totalDiscount);

    printf("\n[3] BED OCCUPANCY PER WARD\n");
    for (int w = 0; w < NUM_WARDS; w++) {
        printf("    %-18s : %.1f%%\n",
               wardNames[w], wardOccupancyPercent(wardIDs[w]));
    }
    printf("\n[4] HIGHEST-PAYING PATIENT\n");
    printf("    Name                      : %s\n", highestName);
    printf("    Total Bill                : LKR %.2f\n", highestBill);

    printSeparator();
}
void saveBedStatus(void) {
    FILE *fp = fopen("beds_status.txt", "w");
    if (!fp) { printf(">> Could not save beds_status.txt\n"); return; }

    for (int w = 0; w < NUM_WARDS; w++) {
        for (int b = 0; b < BEDS_PER_WARD; b++)
            fprintf(fp, "%d ", bedOccupancy[w][b]);
        fprintf(fp, "\n");
    }
    fclose(fp);
}
void loadBedStatus(void) {
    FILE *fp = fopen("beds_status.txt", "r");
    if (!fp) return;                        /* no file yet - keep zeros */

    for (int w = 0; w < NUM_WARDS; w++) {
        for (int b = 0; b < BEDS_PER_WARD; b++) {
            if (fscanf(fp, "%d", &bedOccupancy[w][b]) != 1) {
                fclose(fp);
                return;
            }
        }
    }
    fclose(fp);
}
void appendPatientRecord(int idx) {
    FILE *fp = fopen("patient_records.txt", "a");
    if (!fp) { printf(">> Could not write patient_records.txt\n"); return; }

    fprintf(fp,
        "PAT-%d|%s|Age:%d|Urg:%d|Spec:%s|Ward:%s|Days:%d|"
        "Base:%.2f|Surcharge:%.2f|WardCost:%.2f|Gross:%.2f|"
        "Discount:%.2f|Final:%.2f|Wait:%.2f\n",
        patientIDs[idx], patientNames[idx], patientAges[idx],
        patientUrgency[idx], specialtyNames[patientSpecialty[idx] - 1],
        patientAdmitted[idx] ? wardNames[patientWard[idx] - 1] : "OPD",
        patientDays[idx], patientBaseFee[idx], patientSurcharge[idx],
        patientWardCost[idx], patientGross[idx], patientDiscount[idx],
        patientFinal[idx], patientWaitTime[idx]);

    fclose(fp);
}
