
#ifndef DEMO_FINITESTATEMACHINE_H
#define DEMO_FINITESTATEMACHINE_H


#define FSM_STATE(X) \
	void (*X)();\
	uint8_t (*is##X)();\
	uint8_t (*isNot##X)();\
	void (*set##X)();

#define FSM_STATE_FUNCS(X) \
	static void state##X();\
	inline static uint8_t is##X(){ return fsmState == StateMachine.X; }\
	inline static uint8_t isNot##X() { return fsmState != StateMachine.X; }\
	inline static void set##X() {fsmState = StateMachine.X;}

//Allow to define struct name
#define FSM_STATE_FUNCTIONS(S,X) \
	static void state##X();\
	inline static uint8_t is##X(){ return fsmState == S.X; }\
	inline static uint8_t isNot##X() { return fsmState != S.X; }\
	inline static void set##X() {fsmState = S.X;}

#define FSM_STATE_INIT(X) \
	StateMachine.X = &state##X;\
	StateMachine.is##X = &is##X;\
	StateMachine.isNot##X = &isNot##X;\
	StateMachine.set##X = &set##X;    \

#define FSM_STATE_INIT_STRUCT(S,X) \
	S.X = &state##X;\
	S.is##X = &is##X;\
	S.isNot##X = &isNot##X;\
	S.set##X = &set##X;\

#define FSM_DEFINE_STATE_MACHINE \
    static void (*fsmState)();\
    struct

#define FIRST_ARG_(N, ...) N

#define CREATE_STRUCT_IMPL_1(S1) FSM_STATE( S1 );
#define CREATE_STRUCT_IMPL_2(S1, ...) CREATE_STRUCT_IMPL_1(S1) CREATE_STRUCT_IMPL_1(__VA_ARGS__)
#define CREATE_STRUCT_IMPL_3(S1, ...) CREATE_STRUCT_IMPL_1(S1) CREATE_STRUCT_IMPL_2(__VA_ARGS__)
#define CREATE_STRUCT_IMPL_4(S1, ...) CREATE_STRUCT_IMPL_1(S1) CREATE_STRUCT_IMPL_3(__VA_ARGS__)
#define CREATE_STRUCT_IMPL_5(S1, ...) CREATE_STRUCT_IMPL_1(S1) CREATE_STRUCT_IMPL_4(__VA_ARGS__)

#define CREATE_STRUCT_IMPL(_1,_2,_3,_4,_5,NAME,...) NAME

#define CREATE_FSTRUCT_IMPL_1(STRUCT_NAME, S1) FSM_STATE_FUNCTIONS(STRUCT_NAME, S1 );
#define CREATE_FSTRUCT_IMPL_2(STRUCT_NAME, S1, ...) CREATE_FSTRUCT_IMPL_1(STRUCT_NAME,S1) CREATE_FSTRUCT_IMPL_1(STRUCT_NAME,__VA_ARGS__)
#define CREATE_FSTRUCT_IMPL_3(STRUCT_NAME, S1, ...) CREATE_FSTRUCT_IMPL_1(STRUCT_NAME,S1) CREATE_FSTRUCT_IMPL_2(STRUCT_NAME,__VA_ARGS__)
#define CREATE_FSTRUCT_IMPL_4(STRUCT_NAME, S1, ...) CREATE_FSTRUCT_IMPL_1(STRUCT_NAME,S1) CREATE_FSTRUCT_IMPL_3(STRUCT_NAME,__VA_ARGS__)
#define CREATE_FSTRUCT_IMPL_5(STRUCT_NAME, S1, ...) CREATE_FSTRUCT_IMPL_1(STRUCT_NAME,S1) CREATE_FSTRUCT_IMPL_4(STRUCT_NAME,__VA_ARGS__)

#define CREATE_FSTRUCT_IMPL(_1,_2,_3,_4,_5,NAME,...) NAME

#define CREATE_INIT_IMPL_1(STRUCT_NAME, S1) FSM_STATE_INIT_STRUCT(STRUCT_NAME, S1 );
#define CREATE_INIT_IMPL_2(STRUCT_NAME, S1, ...) CREATE_INIT_IMPL_1(STRUCT_NAME,S1) CREATE_INIT_IMPL_1(STRUCT_NAME,__VA_ARGS__)
#define CREATE_INIT_IMPL_3(STRUCT_NAME, S1, ...) CREATE_INIT_IMPL_1(STRUCT_NAME,S1) CREATE_INIT_IMPL_2(STRUCT_NAME,__VA_ARGS__)
#define CREATE_INIT_IMPL_4(STRUCT_NAME, S1, ...) CREATE_INIT_IMPL_1(STRUCT_NAME,S1) CREATE_INIT_IMPL_3(STRUCT_NAME,__VA_ARGS__)
#define CREATE_INIT_IMPL_5(STRUCT_NAME, S1, ...) CREATE_INIT_IMPL_1(STRUCT_NAME,S1) CREATE_INIT_IMPL_4(STRUCT_NAME,__VA_ARGS__)

#define CREATE_INIT_IMPL(_1,_2,_3,_4,_5,NAME,...) NAME

/**
 * Name of the state machine
 * List of states
 */
#define CREATE_STATE_MACHINE(N, ...) \
    FSM_DEFINE_STATE_MACHINE {       \
    void (*update)();\
    CREATE_STRUCT_IMPL(__VA_ARGS__, CREATE_STRUCT_IMPL_5, CREATE_STRUCT_IMPL_4, CREATE_STRUCT_IMPL_3, CREATE_STRUCT_IMPL_2, CREATE_STRUCT_IMPL_1)(__VA_ARGS__) } N;\
    CREATE_FSTRUCT_IMPL(__VA_ARGS__, CREATE_FSTRUCT_IMPL_5, CREATE_FSTRUCT_IMPL_4, CREATE_FSTRUCT_IMPL_3, CREATE_FSTRUCT_IMPL_2, CREATE_FSTRUCT_IMPL_1)(N,__VA_ARGS__)           \
    inline static void update##N() { fsmState(); }                                 \
    static void init##N() {          \
        CREATE_INIT_IMPL(__VA_ARGS__, CREATE_INIT_IMPL_5, CREATE_INIT_IMPL_4, CREATE_INIT_IMPL_3, CREATE_INIT_IMPL_2, CREATE_INIT_IMPL_1)(N,__VA_ARGS__) \
        fsmState = N.FIRST_ARG_(__VA_ARGS__);                                                                                                                      \
        N.update = &update##N;\
    }


#endif //DEMO_FINITESTATEMACHINE_H
