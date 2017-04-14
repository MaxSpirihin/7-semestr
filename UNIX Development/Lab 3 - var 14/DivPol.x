const  POLYNOM_MAX_POWER=10;

struct Polynom {
        int power;
        double coefs[POLYNOM_MAX_POWER];
};

struct DivPolRequest {
        Polynom source;
        Polynom divider;
};

struct DivPolResponse {
        Polynom result;
        Polynom remainder;
};

program DIVPOL_PROG {
        version DIVPOL_VERS {
                DivPolResponse DIVPOL(DivPolRequest) = 1;
        } = 1;
} = 0x23451111;
