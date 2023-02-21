#include "main.h"

void LED_Init();
void Button_Init();

void EXTI0_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(BUTTON_PIN);
}

uint8_t isDebouncing = 0;
uint32_t timeOfEdge = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (isDebouncing == 0) {
        isDebouncing = 1;
        timeOfEdge = HAL_GetTick();
    }
}

#define DEBOUNCE_CHECK_MS 5 // Poll hardware interval
#define DEBOUNCE_PRESS_MS 10 // Time until press/release registers

int main(void) {
    HAL_Init();
    LED_Init();
    Button_Init();

    uint32_t prevTime = 0U;
    uint8_t ledTurn = 0U;
    uint32_t nowTime;
    uint32_t debouncePrevTime = 0U;
    uint8_t buttonPressed = 0; // 0 = released, 1 = pressed
    
    while (1) {
        nowTime = HAL_GetTick();

        if (ledTurn == 0 && nowTime - prevTime > 50) {
            HAL_GPIO_TogglePin(LED_GPIO_PORT, GREEN_LED_PIN);
            prevTime = nowTime;
            ledTurn = 1;
        }

        if (ledTurn == 1 && nowTime - prevTime > 50) {
            HAL_GPIO_TogglePin(LED_GPIO_PORT, BLUE_LED_PIN);
            prevTime = nowTime;
            ledTurn = 2;
        }

        if (ledTurn == 2 && nowTime - prevTime > 50) {
            HAL_GPIO_TogglePin(LED_GPIO_PORT, RED_LED_PIN);
            prevTime = nowTime;
            ledTurn = 0;
        }

        // if (ledTurn == 3 && nowTime - prevTime > 50) {
        //     HAL_GPIO_TogglePin(LED_GPIO_PORT, ORANGE_LED_PIN);
        //     prevTime = nowTime;
        //     ledTurn = 0;
        // }

        // 
        if (isDebouncing && nowTime - debouncePrevTime > DEBOUNCE_CHECK_MS) {
            if (nowTime - timeOfEdge > DEBOUNCE_PRESS_MS) {
                isDebouncing = 0;
                buttonPressed = HAL_GPIO_ReadPin(BUTTON_GPIO_PORT, BUTTON_PIN);
                HAL_GPIO_TogglePin(LED_GPIO_PORT, ORANGE_LED_PIN);
            }
            debouncePrevTime = nowTime;
        }

        if (buttonPressed == 1) {
            // "Freeze" time if button is pressed
            prevTime = nowTime;
        }

    }
}

void LED_Init() {
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GREEN_LED_PIN | ORANGE_LED_PIN | RED_LED_PIN | BLUE_LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
}

void Button_Init() {
    // VERY IMPORTANT TO ENABLE CLOCK! Otherwise ReadPin doesn't work!
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void SysTick_Handler(void) {
    HAL_IncTick();
}