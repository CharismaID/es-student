#include "memory.h"
#include "command.h"
#include "device.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/regs/addressmap.h"

int main(void);

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

uint32_t data_variable = 100;
uint32_t bss_variable;

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

void fw_info(void)
{
    data_variable++;
    bss_variable++;

    uint16_t *main_code = (uint16_t *)((uintptr_t)main & ~1u);
    uint16_t *fw_code = (uint16_t *)((uintptr_t)fw_info & ~1u);

    uint32_t stack_variable = 1946;
    uint32_t *heap_variable = malloc(sizeof(uint32_t));
    if (heap_variable != NULL)
    {
        *heap_variable = 1951;
    }

    printf("object          address     value\n");
    printf("%-15s 0x%08x  0x%04x\n", "main", (unsigned)(uintptr_t)main, (unsigned)*main_code);
    printf("%-15s 0x%08x  0x%04x\n", "fw_info", (unsigned)(uintptr_t)fw_info, (unsigned)*fw_code);
    printf("%-15s 0x%08x\n", "commands", (unsigned)(uintptr_t)commands);

    for (uint i = 0; i < command_count; i++)
    {
        printf("- %-13s 0x%08x\n", commands[i].name, (unsigned)(uintptr_t)commands[i].handler);
    }

    printf("%-15s 0x%08x  %s\n", "DEVICE_PROJECT", (unsigned)(uintptr_t)DEVICE_PROJECT, DEVICE_PROJECT);
    printf("%-15s 0x%08x  %s\n", "DEVICE_BOARD", (unsigned)(uintptr_t)DEVICE_BOARD, DEVICE_BOARD);
    printf("%-15s 0x%08x  %u\n", "data_variable", (unsigned)(uintptr_t)&data_variable, (unsigned)data_variable);
    printf("%-15s 0x%08x  %u\n", "bss_variable", (unsigned)(uintptr_t)&bss_variable, (unsigned)bss_variable);
    printf("%-15s 0x%08x  %u\n", "stack_variable", (unsigned)(uintptr_t)&stack_variable, (unsigned)stack_variable);
    if (heap_variable != NULL)
    {
        printf("%-15s 0x%08x  %u\n", "heap_variable", (unsigned)(uintptr_t)heap_variable, (unsigned)*heap_variable);
        free(heap_variable);
    }
}
