#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Account{
    int acc;
    char name[30];
    int pin;
    float bal;
};

//Transaction History 
void saveHistory(int acc, float amt, char type[]) {
    FILE *h = fopen("history.txt", "a");
    fprintf(h, "%d %s %.2f\n", acc, type, amt);
    fclose(h);
}

void viewHistory(int acc) {
    FILE *h = fopen("history.txt", "r");
    int a; char t[20]; float am;
    printf("\n--- Transaction History ---\n");
    while(fscanf(h,"%d %s %f",&a,t,&am) != EOF) {
        if(a == acc)
            printf("%s  %.2f\n", t, am);
    }
    fclose(h);
}

//Blocked Accounts
int isBlocked(int acc) {
    int x;
    FILE *b = fopen("blocked.txt", "r");
    if(!b) return 0;
    while(fscanf(b, "%d", &x) != EOF)
        if(x == acc) { fclose(b); return 1; }
    fclose(b);
    return 0;
}

//Account Management
void createAccount() {
    FILE *f = fopen("accounts.txt", "a");
    struct Account a;

    printf("\nEnter Account No: ");
    scanf("%d", &a.acc);
    printf("Enter Name: ");
    scanf("%s", a.name);
    printf("Set PIN: ");
    scanf("%d", &a.pin);
    a.bal = 0;

    fprintf(f,"%d %s %d %.2f\n", a.acc, a.name, a.pin, a.bal);
    fclose(f);
    printf("Account created successfully!\n");
}

int login(struct Account *p) {
    FILE *f = fopen("accounts.txt", "r");
    int acc, pin;
    printf("\nEnter Account No: "); scanf("%d", &acc);

    if(isBlocked(acc)) {
        printf("\nYour account is blocked. Contact admin.\n");
        return 0;
    }

    printf("Enter PIN: "); scanf("%d", &pin);

    while(fscanf(f,"%d %s %d %f",&p->acc,p->name,&p->pin,&p->bal)!=EOF)
        if(p->acc==acc && p->pin==pin) { fclose(f); return 1; }

    fclose(f);
    printf("Invalid account or PIN!\n");
    return 0;
}

void updateAccount(struct Account a) {
    FILE *f = fopen("accounts.txt", "r");
    FILE *tmp = fopen("temp.txt", "w");
    struct Account x;

    while(fscanf(f,"%d %s %d %f",&x.acc,x.name,&x.pin,&x.bal)!=EOF) {
        if(x.acc==a.acc)
            fprintf(tmp,"%d %s %d %.2f\n", a.acc,a.name,a.pin,a.bal);
        else
            fprintf(tmp,"%d %s %d %.2f\n", x.acc,x.name,x.pin,x.bal);
    }
    fclose(f); fclose(tmp);
    remove("accounts.txt"); rename("temp.txt","accounts.txt");
}

//User Operations
void withdraw(struct Account *a) {
    float amt;
    printf("\nEnter amount: "); scanf("%f",&amt);
    if(amt > a->bal) { printf("Insufficient balance!\n"); return; }
    a->bal -= amt;
    updateAccount(*a);
    saveHistory(a->acc, amt, "Withdraw");
    printf("Withdraw successful.\n");
}

void deposit(struct Account *a) {
    float amt;
    printf("\nEnter amount: "); scanf("%f",&amt);
    a->bal += amt;
    updateAccount(*a);
    saveHistory(a->acc, amt, "Deposit");
    printf("Deposit successful.\n");
}

void transfer(struct Account *a) {
    int to; float amt; struct Account r; FILE *f;
    printf("\nEnter receiver account: "); scanf("%d",&to);
    printf("Enter amount: "); scanf("%f",&amt);

    if(amt > a->bal) { printf("Not enough balance!\n"); return; }

    f = fopen("accounts.txt","r");
    while(fscanf(f,"%d %s %d %f",&r.acc,r.name,&r.pin,&r.bal)!=EOF) {
        if(r.acc==to) {
            fclose(f);
            a->bal -= amt; updateAccount(*a);
            r.bal += amt; updateAccount(r);
            saveHistory(a->acc, amt, "TransferOut");
            saveHistory(r.acc, amt, "TransferIn");
            printf("Transfer successful.\n");
            return;
        }
    }
    fclose(f);
    printf("Receiver account not found!\n");
}

//Admin Panel
void admin() {
    int ch, acc; struct Account a; FILE *f;
    char password[20];
    printf("\nEnter admin password: "); scanf("%s",password);
    if(strcmp(password,"admin123")!=0) { printf("Wrong password!\n"); return; }

    do {
        printf("\n--- ADMIN PANEL ---\n");
        printf("1. View all accounts\n2. View transaction history\n");
        printf("3. Block/Delete account\n4. Create account\n5. Exit admin\n");
        printf("Enter choice: "); scanf("%d",&ch);

        if(ch==1) {
            f = fopen("accounts.txt","r");
            printf("\nACCNO  NAME  PIN  BAL\n");
            while(fscanf(f,"%d %s %d %f",&a.acc,a.name,&a.pin,&a.bal)!=EOF)
                printf("%d  %s  %d  %.2f\n",a.acc,a.name,a.pin,a.bal);
            fclose(f);
        }
        else if(ch==2) { printf("Enter account: "); scanf("%d",&acc); viewHistory(acc); }
        else if(ch==3) {
            FILE *b = fopen("blocked.txt","a");
            printf("Enter account to block: "); scanf("%d",&acc);
            fprintf(b,"%d\n",acc); fclose(b);
            printf("Account blocked!\n");
        }
        else if(ch==4) createAccount();

    } while(ch!=5);
}

//Main Function
int main() {
    int ch; struct Account user;

    while(1) {
        printf("\n--- ATM SYSTEM ---\n");
        printf("1. Create Account\n2. Login\n3. Admin Panel\n4. Exit\n");
        printf("Enter choice: "); scanf("%d",&ch);

        if(ch==1) createAccount();
        else if(ch==2) {
            if(login(&user)) {
                int c;
                do {
                    printf("\n--- ACCOUNT MENU ---\n");
                    printf("1. Balance\n2. Withdraw\n3. Deposit\n4. Transfer\n5. History\n6. Logout\n");
                    printf("Choice: "); scanf("%d",&c);

                    if(c==1) printf("Balance: %.2f\n",user.bal);
                    else if(c==2) withdraw(&user);
                    else if(c==3) deposit(&user);
                    else if(c==4) transfer(&user);
                    else if(c==5) viewHistory(user.acc);

                } while(c!=6);
            }
        }
        else if(ch==3) admin();
        else if(ch==4) break;
    }
    return 0;
}
