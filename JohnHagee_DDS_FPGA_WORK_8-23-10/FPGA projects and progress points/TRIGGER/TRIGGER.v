module TRIGGER(int_50MHz_clock, key_0, trigger_rabbit, trigger_scope);

input int_50MHz_clock;
input key_0;
output reg trigger_rabbit;
output reg trigger_scope;


always@ (posedge int_50MHz_clock)
	begin
	if (key_0 == 1)
		begin
		trigger_rabbit<=0;
		trigger_scope<=0;
		end
			
	if (key_0 == 0)
		begin
		trigger_rabbit <= 1;
		trigger_scope <= 1;
		end
		
	end
endmodule
