

main()

{

  /* variables and pointers to test RGMII interface */
  int i;
  volatile unsigned long rgmiistatus;
  volatile unsigned long rgmiibuf[256];
  volatile unsigned long* rgmiistatusp = (unsigned long*) 0x80001000;
  volatile unsigned long* rgmiiop = (unsigned long*) 0x80001400;
  volatile unsigned long* rgmiiip = (unsigned long*) 0x80001800;

  report_start();
  
  base_test();
      
  greth_test(0x800c0000);

  report_end();
}
