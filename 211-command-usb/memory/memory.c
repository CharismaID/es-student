#include "memory.h"

#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/regs/addressmap.h"

extern char __flash_binary_start;
extern char __flash_binary_end;
extern char __boot2_start__;
extern char __boot2_end__;
extern char __etext;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __HeapLimit;
extern char __StackBottom;
extern char __StackTop;

static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}

void mem_info(void)
{
    uintptr_t flash_start = (uintptr_t)XIP_BASE;
    uintptr_t flash_end = flash_start + PICO_FLASH_SIZE_BYTES;

    uintptr_t sram_start = (uintptr_t)SRAM_BASE;
    uintptr_t sram_end = (uintptr_t)SRAM_END;

    uintptr_t rom_start = (uintptr_t)ROM_BASE;
    uintptr_t rom_end = rom_start + 16384;

    uintptr_t img_start = (uintptr_t)&__flash_binary_start;
    uintptr_t img_end = (uintptr_t)&__flash_binary_end;

    uintptr_t boot2_start = (uintptr_t)&__boot2_start__;
    uintptr_t boot2_end = (uintptr_t)&__boot2_end__;

    uintptr_t text_start = (uintptr_t)&__boot2_end__;
    uintptr_t text_end = (uintptr_t)&__etext;

    uintptr_t data_ram_start = (uintptr_t)&__data_start__;
    uintptr_t data_ram_end = (uintptr_t)&__data_end__;
    uintptr_t data_len = data_ram_end - data_ram_start;

    uintptr_t data_flash_start = (uintptr_t)&__etext;
    uintptr_t data_flash_end = data_flash_start + data_len;

    uintptr_t bss_start = (uintptr_t)&__bss_start__;
    uintptr_t bss_end = (uintptr_t)&__bss_end__;

    uintptr_t heap_start = (uintptr_t)&__bss_end__;
    uintptr_t heap_end = (uintptr_t)&__HeapLimit;

    uintptr_t stack_start = (uintptr_t)&__StackBottom;
    uintptr_t stack_end = (uintptr_t)&__StackTop;

    printf("area       start      end        size\n");
    row("flash", flash_start, flash_end);
    row("sram", sram_start, sram_end);
    row("rom", rom_start, rom_end);
    row("image", img_start, img_end);
    row("free", img_end, flash_end);
    row("boot2", boot2_start, boot2_end);
    row("text", text_start, text_end);
    row("data flash", data_flash_start, data_flash_end);
    row("data ram", data_ram_start, data_ram_end);
    row("bss", bss_start, bss_end);
    row("heap", heap_start, heap_end);
    row("stack", stack_start, stack_end);

    unsigned boot2_sz = (unsigned)(boot2_end - boot2_start);
    unsigned text_sz = (unsigned)(text_end - text_start);
    unsigned data_sz = (unsigned)data_len;
    unsigned image_sz = (unsigned)(img_end - img_start);
    unsigned bss_sz = (unsigned)(bss_end - bss_start);
    unsigned heap_sz = (unsigned)(heap_end - heap_start);
    unsigned stack_sz = (unsigned)(stack_end - stack_start);

    printf("\ntotal\n");
    printf("  flash image %8u = boot2 %u + text %u + data %u\n",
           image_sz, boot2_sz, text_sz, data_sz);
    printf("  flash free  %8u of %u\n",
           (unsigned)(flash_end - img_end), (unsigned)PICO_FLASH_SIZE_BYTES);
    printf("  ram used    %8u = data %u + bss %u\n",
           data_sz + bss_sz, data_sz, bss_sz);
    printf("  ram free    %8u for heap and %u for stack\n",
           heap_sz, stack_sz);
}
