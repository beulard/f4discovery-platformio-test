#include "main.h"

void LED_Init();
void Button_Init();

int main(void) {
    HAL_Init();
    LED_Init();
    Button_Init();

    uint32_t prevTime = 0U;
    uint8_t ledTurn = 0;
    uint32_t nowTime;
    uint8_t buttonPressed = 0;
    
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
            ledTurn = 3;
        }

        if (ledTurn == 3 && nowTime - prevTime > 50) {
            HAL_GPIO_TogglePin(LED_GPIO_PORT, ORANGE_LED_PIN);
            prevTime = nowTime;
            ledTurn = 0;
        }

        GPIO_PinState buttonState = HAL_GPIO_ReadPin(BUTTON_GPIO_PORT, BUTTON_PIN);
        if (buttonState == GPIO_PIN_SET) {
            buttonPressed = 1;
        } else if (buttonState == GPIO_PIN_RESET) {
            buttonPressed = 0;
        }
        if (buttonPressed == 1) {
            // "Freeze" time if button is pressed
            prevTime = nowTime;
        }

    }
}

void LED_Init() {
    LED_GPIO_CLK_ENABLE();
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
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStruct);
}

void SysTick_Handler(void) {
    HAL_IncTick();
}