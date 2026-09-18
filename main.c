#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PATIENTS 100
#define NUM_SPECIALTIES 4
#define NUM_WARDS 4
#define MAX_BEDS 20
const char specialtyName[NUM_SPECIALTIES][30] = {
    "General Practice (OPD)", "Paediatrics", "Cardiology", "Neurology"
};
const float baseFee[NUM_SPECIALTIES]     = {1500.00, 2500.00, 4500.00, 5000.00};
const int   consultTime[NUM_SPECIALTIES] = {15, 20, 30, 30};
const int   dailyCap[NUM_SPECIALTIES]    = {30, 20, 12, 10};

const char wardName[NUM_WARDS][30] = {
    "General Ward", "Paediatric Ward", "Surgical Ward", "ICU (Intensive Care Unit)"
};
const float dailyBedRate[NUM_WARDS] = {3000.00, 6000.00, 12000.00, 25000.00};
const int   bedCapacity[NUM_WARDS]  = {20, 10, 10, 5};

void showMenu(void);

int main() {
    int choice;



    do {
        showMenu();
        if (scanf("%d", &choice) != 1) {

            while (getchar() != '\n');
            choice = -1;
        }



        } while (choice != 5);

    return 0;


}
void showMenu(void) {
    printf("\n==================================================\n");
    printf("     SMART HOSPITAL PATIENT & RESOURCE SYSTEM\n");
    printf("==================================================\n");
    printf("1. Register New Patient\n");
    printf("2. View Bed Occupancy Status\n");
    printf("3. Display Patients by Priority (Triage Order)\n");
    printf("4. Generate Performance Report\n");
    printf("5. Exit\n");
    printf("--------------------------------------------------\n");
    printf("Enter your choice: ");
}


