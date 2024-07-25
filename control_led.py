import time

def toggle_led(led_path, state):
    with open(led_path, 'w') as led:
        led.write(state)
        led.flush()

def main():
    led0_path = '/dev/LED0'
    led1_path = '/dev/LED1'
    
    while True:
        
        toggle_led(led0_path, '1')
        toggle_led(led1_path, '0')
        time.sleep(1)
        
        toggle_led(led0_path, '0')
        toggle_led(led1_path, '1')
        time.sleep(1)

if __name__ == '__main__':
    main()
