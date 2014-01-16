module DE0_trigger(int_clock,push_button_2, trigger_out, trig_flag);
input push_button_2;
input int_clock;
output reg trigger_out;
integer T;
output reg trig_flag;
initial
	begin
	trig_flag<=0;
	trigger_out<=0;
	end

always@ (posedge int_clock)
	begin
	if (push_button_2==0)
		begin
		trig_flag<=1;
		end
	
	
	if (trig_flag!=0)
		begin
		T<=T+1;
		end
	if ((T>=1) && (T<=25))
		begin
		trigger_out<=1;
		end
	if ((T<1) || (T>25))
		begin
		trigger_out<=0;
		end
	if (T>25000000)
		begin
		T<=0;
		trig_flag<=0;
		end
	end
endmodule
