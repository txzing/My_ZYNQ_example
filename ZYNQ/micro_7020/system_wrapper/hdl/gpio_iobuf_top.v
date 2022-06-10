module gpio_iobuf_top
#(parameter  DATA_WIDTH = 3)
(
     input    [(DATA_WIDTH-1):0]  gpio_t   ,
     input    [(DATA_WIDTH-1):0]  gpio_i  ,
     output   [(DATA_WIDTH-1):0]  gpio_o   ,
     inout                        gpio_intr,
     inout                        key_0    ,
     inout                        key_1

);



gpio_iobuf 
     #(.DATA_WIDTH(DATA_WIDTH)) 
     i_iobuf 
     (
    .dio_t ({gpio_t[DATA_WIDTH-1:0]}),
    .dio_i ({gpio_o[DATA_WIDTH-1:0]}),
    .dio_o ({gpio_i[DATA_WIDTH-1:0]}),
    .dio_p ({ gpio_intr,   
              key_0    ,
              key_1    
            })    
         );

endmodule   
