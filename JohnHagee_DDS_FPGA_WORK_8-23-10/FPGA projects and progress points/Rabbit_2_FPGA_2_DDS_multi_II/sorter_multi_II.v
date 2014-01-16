module sorter_multi_II(ten_MHz_ext_2,full_184_bit_in, key_1_trigger_2,key_3_sweep_2, full_184_bit_out)







always@(posedge ten_MHz_ext_2)
	begin
	if ((key_1_trigger_2 == 0) && (trigger_ok_flag == 1))
		begin
		N<=N+1;
		end
		
	//if ((key_3_sweep_2 == 0) && (sweep_ok_flag == 1))
		//begin
		//M<=M+1;
		//end
		
	led_display<=N;	
	if (L<1)
		begin
		L<=L+1;
		counter<=0;
		end
	if (L>=1)
		begin
		L<=0
		counter<=1;
		end
	end
	
always@ (negedge ten_MHz_ext_2)
	begin
	if (key_1_trigger_2 == 1)
		begin
		trigger_ok_flag<=1;
		
		end
	
	
	//if (key_3_sweep_2 == 1)
		//begin
		//sweep_ok_flag<=1:
		//end
	if ((key_1_trigger_2 == 0)&&(counter == 0))
		begin
		trigger_ok_flag<=0;
		end
	//if (key_3_sweep_2 == 0)
		//begin
		//sweep_ok_flag<=0:
		//end
	end
		
		
		
	