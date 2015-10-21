/* Uniwersalna procedura startowa dla STM32 */

/* Następujące zmienne są zdefiniowane w skrypcie konsolidatora. */
extern unsigned long _sidata;   /* pierwsze 4 bajty danych
                                   inicjowanych w FLASH */
extern unsigned long _sdata;    /* pierwsze 4 bajty danych
                                   inicjowanych w RAM */
extern unsigned long _edata;    /* 4 bajty tuż za danymi
                                   inicjowanymi w RAM */
extern unsigned long _sbss;     /* pierwsze 4 bajty danych
                                   nieinicjowanych w RAM */
extern unsigned long _ebss;     /* 4 bajty tuż za danymi
                                   nieinicjowanymi w RAM */
extern unsigned long _estack;   /* początkowy wierzchołek stosu */

/* Deklaracja głównej funkcji programu */
int main(void);

/* Domyślna procedura obsługi przerwania - nieoczekiwane
   przerwanie zawiesza mikrokontroler. */
static void Default_Handler(void) {
  for (;;);
}

/* Procedura wołana po wyzerowaniu mikrokontrolera */
static void Reset_Handler(void) {
  unsigned long *src, *dst;
  /* Kopiuj dane inicjowane z FLASH do RAM. */
  for (dst = &_sdata, src = &_sidata; dst < &_edata; ++dst, ++src)
    *dst = *src;
  /* Zeruj dane nieinicjowane. */
  for (dst = &_sbss; dst < &_ebss; ++dst)
    *dst = 0;
  /* Wołaj główną funkcję programu - zwykle nigdy nie wraca. */
  main();
  /* Gdyby jednak nastąpił powrót, kręć się w kółko. */
  for (;;);
}

/* Poniższy kod i włączane pliki są wygenerowane automatycznie. */
#if defined STM32F0XX
  #include <startup_stm32f0xx.c>
#elif defined STM32F10X_CL
  #include <startup_stm32f10x_cl.c>
#elif defined STM32F10X_HD
  #include <startup_stm32f10x_hd.c>
#elif defined STM32F10X_HD_VL
  #include <startup_stm32f10x_hd_vl.c>
#elif defined STM32F10X_LD
  #include <startup_stm32f10x_ld.c>
#elif defined STM32F10X_LD_VL
  #include <startup_stm32f10x_ld_vl.c>
#elif defined STM32F10X_MD
  #include <startup_stm32f10x_md.c>
#elif defined STM32F10X_MD_VL
  #include <startup_stm32f10x_md_vl.c>
#elif defined STM32F10X_XL
  #include <startup_stm32f10x_xl.c>
#elif defined STM32F2XX
  #include <startup_stm32f2xx.c>
#elif defined STM32F30X
  #include <startup_stm32f30x.c>
#elif defined STM32F37X
  #include <startup_stm32f37x.c>
#elif defined STM32F4XX
  #include <startup_stm32f4xx.c>
#elif defined STM32L1XX_HD
  #include <startup_stm32l1xx_hd.c>
#elif defined STM32L1XX_MDP
  #include <startup_stm32l1xx_mdp.c>
#elif defined STM32L1XX_MD
  #include <startup_stm32l1xx_md.c>
#else
  #error Device type is not defined.
#endif
