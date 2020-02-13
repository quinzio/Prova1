// ex17


typedef enum
{
    IDLE,       /* !< Waiting for Initial starting conditions        */
    PARK,       /* !< Parking                                        */
    RUN_ON_FLY, /* !< Transition State that allow Run_On_Fly looking */
    RUN,        /* !< Running in normal condition                    */
    WAIT,       /* !< Waiting for next start up                      */
    FAULT       /* !< Fault state for errors or invalid parameters   */
} SystStatus_t;


enum e1 {
    A,
    B = 10,
    C
};


enum e1 vare1;
int var1;

int f1() {
    var1 = WAIT;
    vare1 = C;

	return 0;
}