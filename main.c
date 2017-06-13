//Global Variables for all state machines
unsigned char up_down, left_right; //hold ud and lr values to set the position
unsigned char position = 0;		//1 up 2 down 3 left 4 right 0 nothing
unsigned char instruction = 0;	//1 up 2 down 3 left 4 right 5 nothing

const short MAX = 165;	//calculated specific to my joystick
const short MIN = 30;	//calculated specific to my joystick

unsigned char cnt = 0;	//global count that is reset in every SM's init state that it's used in

unsigned char wrong = 0; //wrong moves made
unsigned char correct = 0; //correct moves made
const unsigned char TOTAL_GAME_STATES = 13;

enum ADCReader_States { ADCReader_init, wait }; //reads joystick input and sets up_down & left_right values
 
int TickFct_ADCReader(int state) {

	// === Transitions ===
	switch (state) {
	case ADCReader_init:
		state = ADCReader_init;
		break;

	case wait:
		if (cnt < 3){ state = wait; }
		else { state = ADCReader_init; }
		break;

	default:
		state = ADCReader_init;
		break;
	}

	// === Actions ===
	switch (state) {
	case ADCReader_init:
		up_down = ADCNum(0);
		left_right = ADCNum(1);
		cnt = 0;
		break;

	case wait:
		cnt = cnt + 1;
		break;
	}
	return state;
}
 
enum InputTranslator_States { InputTranslator_init }; //translates joystick's direction values from (up_down & left_right) to (position)
 
int TickFct_InputTranslator(int state) {

	// === Transitions ===
	switch (state) {
	case InputTranslator_init:
		state = InputTranslator_init;
		break;
	default:
		state = InputTranslator_init;
		break;
	}

	// === Actions ===
	switch (state) {
	case InputTranslator_init:
		if (left_right > MAX) { position = 3; } //left
		else if (up_down > MAX) { position = 1; } //up
		else if (up_down < MIN) { position = 2; } //down
		else if (left_right < MIN) { position = 4; } //right
		else { position = 0; }
		break;
	default:
		break;
	}
	return state;
}

 
enum RGB_States {RGB_init, countdown3, countdown2, countdown1, countdownGO,
	ud_display, down_display, lr_display, right_display,
	end_display}; 
                    //changes instructions if correct input is entered
 
int TickFct_RGB(int state) {
    // === Local Variables ===
    static unsigned char column_val = 0x01; // (COLOR)sets the pattern displayed on columns
    static unsigned char column_sel = 0x7F; // (GROUND)grounds column to display pattern
    unsigned char button, start_button = 0x00; // (RESET) and (START) button
    button = ~PINA & 0x08; // if(button == 0x08) {state = RGB_init;} resets
    start_button = ~PINA & 0x10; // if(button == 0x10) {state = ud_display;} starts
 
	static char cnt = 0;
    // === Transitions ===
    switch (state) {
        case RGB_init:
            if(start_button == 0x10) {state = countdown3;} //(start)
            break;
             
		case countdown3:
			if (cnt < 5){ state = countdown3; }
			else { state = countdown2; }
			
			if (button == 0x08) { state = RGB_init; } //(reset)
			break;

		case countdown2:
			if (cnt < 10){ state = countdown2; }
			else { state = countdown1; }

			if (button == 0x08) { state = RGB_init; } //(reset)
			break;

		case countdown1:
			if (cnt < 15){ state = countdown1; }
			else { state = countdownGO; }

			if (button == 0x08) { state = RGB_init; } //(reset)
			break;

		case countdownGO:
			if (cnt < 19){ state = countdownGO; }
			else { state = ud_display; }

			if (button == 0x08) { state = RGB_init; } //(reset)
			break;

        case ud_display:
            if(button == 0x08) {state = RGB_init;} //(reset)
                 
            if (position == 1) {state = down_display; correct = correct + 1;} //correct
            else if ((position == 2) || (position == 3)) {state = ud_display; wrong = wrong + 1;} //wrong
             
			if ((correct + wrong) == TOTAL_GAME_STATES) { state = end_display; } //total = 13
            break;
             
        case lr_display:
            if(button == 0x08) {state = RGB_init;} //(reset)
                 
            if (position == 3) {state = ud_display; correct = correct + 1;} //correct
            else if ((position == 1) || (position == 2)) {state = lr_display; wrong = wrong + 1;} //wrong
             
			if ((correct + wrong) == TOTAL_GAME_STATES) { state = end_display; } //total = 13
            break;

		case down_display:
			if (button == 0x08) { state = RGB_init; } //(reset)

			if (positon == 2) { state = lr_display; correct = correct + 1; } //correct
			else if ((position == 1) || (position == 3)) { state = down_display; wrong = wrong + 1; } //wrong

			if ((correct + wrong) == TOTAL_GAME_STATES) { state = end_display; } //total = 13
			break;

		//RIGHT NOW RIGHT_DISPLAY IS A DEAD STATE I WILL MAKE IT ACTIVE WHEN I INCORPORATE THE "RIGHT" DIRECTION AND ADD A CHECK FOR (POSITION == 4) **FUTURE WORK**

		case right_display:
			if (button == 0x08) { state = RGB_init; } //(reset)

			if (positon == 4) { state = ud_display; correct = correct + 1; } //correct
			else if ((position == 1) || (position == 3)) { state = right_display; wrong = wrong + 1; } //wrong

			if ((correct + wrong) == TOTAL_GAME_STATES) { state = end_display; } //total = 13
			break;
		
        case end_display:
            if(button == 0x08) {state = RGB_init;} //(reset)
            break;
             
        default:
            state = RGB_init;
            break;
    }
     
    // === Actions ===
	switch (state) {
		case RGB_init: // ILLUMINATE NON-GAME_INSTRUCTION LEDS
			column_sel = 0xC3;
			column_val = 0x3C;
			cnt = 0;
			correct = 0;
			wrong = 0;

			nokia_lcd_clear();
			nokia_lcd_set_cursor(10, 10);
			nokia_lcd_write_string("Follow The", 1);
			nokia_lcd_set_cursor(25, 20);
			nokia_lcd_write_string("Light", 1);
			nokia_lcd_set_cursor(8, 30);
			nokia_lcd_write_string("PRESS START!", 1);

			/*
			nokia_lcd_set_cursor(0, 40);
			nokia_lcd_write_string("LR:", 1);
			nokia_lcd_set_cursor(30, 40);
			nokia_lcd_write_char(ADCNum(1) + '0', 1);

			nokia_lcd_set_cursor(50, 40);
			nokia_lcd_write_string("UD:", 1);
			nokia_lcd_set_cursor(70, 40);
			nokia_lcd_write_char(ADCNum(0) + '0', 1);
			*/

			nokia_lcd_render();
			break;

		case countdown3:
			column_sel = 0xC3;
			column_val = 0x3C;
			cnt = cnt + 1;
			correct = 0;
			wrong = 0;

			nokia_lcd_clear();
			nokia_lcd_set_cursor(40, 20);
			nokia_lcd_write_char(3 + '0', 2);
			nokia_lcd_render();
			break;

		case countdown2:
			column_sel = 0xC3;
			column_val = 0x3C;
			cnt = cnt + 1;
			correct = 0;
			wrong = 0;

			nokia_lcd_clear();
			nokia_lcd_set_cursor(40, 20);
			nokia_lcd_write_char(2 + '0', 2);
			nokia_lcd_render();
			break;

		case countdown1:
			column_sel = 0xC3;
			column_val = 0x3C;
			cnt = cnt + 1;
			correct = 0;
			wrong = 0;

			nokia_lcd_clear();
			nokia_lcd_set_cursor(40, 20);
			nokia_lcd_write_char(1 + '0', 2);
			nokia_lcd_render();
			break;

		case countdownGO:
			column_sel = 0xC3;
			column_val = 0x3C;
			cnt = cnt + 1;
			correct = 0;
			wrong = 0;

			nokia_lcd_clear();
			nokia_lcd_set_cursor(38, 20);
			nokia_lcd_write_string("GO!", 2);
			nokia_lcd_render();
			break;

		case ud_display: // ILLUMINATE TOP AND BOTTOM 2 ROWS
			column_sel = 0x00;
			column_val = 0x03;
			if (button == 0x08) { position = 0; }
			nokia_lcd_clear();
			nokia_lcd_set_cursor(0, 10);
			nokia_lcd_write_string("Correct:", 1);
			if (correct > 9){
				nokia_lcd_set_cursor(55, 10);
				nokia_lcd_write_char((correct / 10) + '0', 1);
			}
			nokia_lcd_set_cursor(60, 10);
			nokia_lcd_write_char((correct % 10) + '0', 1);
			nokia_lcd_set_cursor(0, 20);
			nokia_lcd_write_string("Wrong:", 1);
			if (wrong > 9){
				nokia_lcd_set_cursor(55, 20);
				nokia_lcd_write_char((wrong / 10) + '0', 1);
			}
			nokia_lcd_set_cursor(60, 20);
			nokia_lcd_write_char((wrong % 10) + '0', 1);
			nokia_lcd_set_cursor(30, 40);
			nokia_lcd_write_string("UP", 1);
			nokia_lcd_render();
			break;


		case down_display: // ILLUMINATE BOTTOM 2 ROWS
			column_sel = 0x00;
			column_val = 0xC0; // bottom column_val = 0xC0; up_bottom column_val = 0xC3
			if (button == 0x08) { position = 0; }

			nokia_lcd_clear();
			nokia_lcd_set_cursor(0, 10);
			nokia_lcd_write_string("Correct:", 1);
			if (correct > 9){
				nokia_lcd_set_cursor(55, 10);
				nokia_lcd_write_char((correct / 10) + '0', 1);
			}
			nokia_lcd_set_cursor(60, 10);
			nokia_lcd_write_char((correct % 10) + '0', 1);
			nokia_lcd_set_cursor(0, 20);
			nokia_lcd_write_string("Wrong:", 1);
			if (wrong > 9){
				nokia_lcd_set_cursor(55, 20);
				nokia_lcd_write_char((wrong / 10) + '0', 1);
			}
			nokia_lcd_set_cursor(60, 20);
			nokia_lcd_write_char((wrong % 10) + '0', 1);
			nokia_lcd_set_cursor(30, 40);
			nokia_lcd_write_string("DOWN", 1);
			nokia_lcd_render();
			break;

		case lr_display: // ILLUMINATE LEFT AND RIGHT 2 ROWS
			column_sel = 0x3F; // left_right = 0x3C
			column_val = 0xFF;
			if (button == 0x08) { position = 0; }

			nokia_lcd_clear();
			nokia_lcd_set_cursor(0, 10);
			nokia_lcd_write_string("Correct:", 1);
			if (correct > 9){
				nokia_lcd_set_cursor(55, 10);
				nokia_lcd_write_char((correct / 10) + '0', 1);
			}
			nokia_lcd_set_cursor(60, 10);
			nokia_lcd_write_char((correct % 10) + '0', 1);
			nokia_lcd_set_cursor(0, 20);
			nokia_lcd_write_string("Wrong:", 1);
			if (wrong > 9){
				nokia_lcd_set_cursor(55, 20);
				nokia_lcd_write_char((wrong / 10) + '0', 1);
			}
			nokia_lcd_set_cursor(60, 20);
			nokia_lcd_write_char((wrong % 10) + '0', 1);
			nokia_lcd_set_cursor(30, 40);
			nokia_lcd_write_string("DOWN", 1);
			nokia_lcd_render();
			break;

		case right_display: // ILLUMINATE RIGHT 2 ROWS
			column_sel = 0xFC; // left_right = 0x3C
			column_val = 0xFF;
			if (button == 0x08) { position = 0; }

			nokia_lcd_clear();
			nokia_lcd_set_cursor(0, 10);
			nokia_lcd_write_string("Correct:", 1);
			if (correct > 9){
				nokia_lcd_set_cursor(55, 10);
				nokia_lcd_write_char((correct / 10) + '0', 1);
			}
			nokia_lcd_set_cursor(60, 10);
			nokia_lcd_write_char((correct % 10) + '0', 1);
			nokia_lcd_set_cursor(0, 20);
			nokia_lcd_write_string("Wrong:", 1);
			if (wrong > 9){
				nokia_lcd_set_cursor(55, 20);
				nokia_lcd_write_char((wrong / 10) + '0', 1);
			}
			nokia_lcd_set_cursor(60, 20);
			nokia_lcd_write_char((wrong % 10) + '0', 1);
			nokia_lcd_set_cursor(30, 40);
			nokia_lcd_write_string("DOWN", 1);
			nokia_lcd_render();
			break;
			
		case end_display: // ILLUMINATE ALL
			column_sel = 0x00;
			column_val = 0xFF;
			if (button == 0x08) { position = 0; }

			nokia_lcd_clear();
			nokia_lcd_set_cursor(20, 0);


			if (correct > wrong) //win if u get more correct than wrong
			{
				nokia_lcd_write_string("YOU WIN!", 1);
				nokia_lcd_set_cursor(0, 10);
				nokia_lcd_write_string("More Correct", 1);

			}
			else
			{
				nokia_lcd_write_string("YOU LOSE!", 1);
				nokia_lcd_set_cursor(0, 10);
				nokia_lcd_write_string("More Wrong", 1);
			}

			nokia_lcd_set_cursor(0, 30);
			nokia_lcd_write_string("Correct:", 1);
			if (correct > 9){
				nokia_lcd_set_cursor(55, 30);
				nokia_lcd_write_char((correct / 10) + '0', 1);
			}
			nokia_lcd_set_cursor(60, 30);
			nokia_lcd_write_char((correct % 10) + '0', 1);
			nokia_lcd_set_cursor(0, 40);
			nokia_lcd_write_string("Wrong:", 1);
			if (wrong > 9){
				nokia_lcd_set_cursor(55, 40);
				nokia_lcd_write_char((wrong / 10) + '0', 1);
			}
			nokia_lcd_set_cursor(60, 40);
			nokia_lcd_write_char((wrong % 10) + '0', 1);
			nokia_lcd_render();
			break;
             
        default:
            break;
    }
     
    PORTC = column_val; // (COLOR) displays column pattern
    PORTD = column_sel; // (GROUND) selects columns to display pattern
    return state;
}
//---------------------------------------------------------------------------------------------------------------------------
int main()
{   
    DDRA = 0x00; PORTA = 0xFF;  //  INPUT from joystick(0-1) reset button(3)
    DDRB = 0xFF; PORTB = 0x00;  //  NOKIA 5110
    DDRD = 0xFF; PORTD = 0x00;  //  ground RGB
    DDRC = 0xFF; PORTC = 0x00;  //  light RGB
     
    // Period for the tasks
    unsigned long int ADCReader_calc = 50;
    unsigned long int InputTranslator_calc = 50;
    unsigned long int RGB_calc = 200;
 
    //GCD Calculation
    unsigned long int tmpGCD = 1;
    tmpGCD = findGCD(ADCReader_calc, InputTranslator_calc);
    tmpGCD = findGCD(tmpGCD, RGB_calc);
     
    //Greatest common divisor for all tasks or smallest time unit for tasks.
    unsigned long int GCD = tmpGCD;
 
    //Recalculate GCD periods for scheduler
    unsigned long int ADCReader_period = ADCReader_calc / GCD;
    unsigned long int InputTranslator_period = InputTranslator_calc / GCD;
    unsigned long int RGB_period = RGB_calc / GCD;
 
    //Declare an array of tasks
    static task task1, task2, task3; 
    task *tasks[] = {&task1, &task2, &task3};
    const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
     
    // Initialize ADC, Timer, LCD
     
    ADC_init();
    TimerSet(GCD);
    TimerOn();
    nokia_lcd_init();
    nokia_lcd_power(1);
     
    // Tasks
    task1.state = ADCReader_init; //Task initial state.
    task1.period = ADCReader_period; //Task Period.
    task1.elapsedTime = ADCReader_period; //Task current elapsed time.
    task1.TickFct = &TickFct_ADCReader; //Function pointer for the tick.
     
    task2.state = InputTranslator_init; 
    task2.period = InputTranslator_period;
    task2.elapsedTime = InputTranslator_period;
    task2.TickFct = &TickFct_InputTranslator;
     
    task3.state = RGB_init;
    task3.period = RGB_period;
    task3.elapsedTime = RGB_period;
    task3.TickFct = &TickFct_RGB;
     
    unsigned short x;
    while (1) {
        for (x = 0; x < numTasks; x++) {
            if (tasks[x]->elapsedTime == tasks[x]->period) {
                tasks[x]->state = tasks[x]->TickFct(tasks[x]->state);
                tasks[x]->elapsedTime = 0;
            }
            tasks[x]->elapsedTime += 1;
        }
        while (!TimerFlag);
        TimerFlag = 0;
    }
 
    return 0;
}
