

void setup()
{

}


void loop()
{

}



#if 0

int led = 13;

#if (SAML21) || (SAML22) || (SAMC20) || (SAMC21)
    #warning "Unsupported"
#elif defined(__SAMD51__)
    #warning "Unsupported"
#endif

// DMA descriptors.
__attribute__((__aligned__(16))) static DmacDescriptor // 128 bit alignment
  _descriptor[DMAC_CH_NUM] SECTION_DMAC_DESCRIPTOR,
  _writeback[DMAC_CH_NUM]  SECTION_DMAC_DESCRIPTOR;

static volatile unsigned long cpu_irq_critical_section_counter = 0;
static volatile unsigned char cpu_irq_prev_interrupt_state     = 0;

uint8_t peripheralTrigger;
dma_transfer_trigger_action triggerAction;

static void cpu_irq_enter_critical(void) {
	if(!cpu_irq_critical_section_counter) {
		if(__get_PRIMASK() == 0) { // IRQ enabled?
			__disable_irq();   // Disable it
			__DMB();
			cpu_irq_prev_interrupt_state = 1;
		} else {
			// Make sure the to save the prev state as false
			cpu_irq_prev_interrupt_state = 0;
		}

	}
	cpu_irq_critical_section_counter++;
}

static void cpu_irq_leave_critical(void) {
	// Check if the user is trying to leave a critical section
	// when not in a critical section
	if(cpu_irq_critical_section_counter > 0) {
		cpu_irq_critical_section_counter--;

		// Only enable global interrupts when the counter
		// reaches 0 and the state of the global interrupt flag
		// was enabled when entering critical state */
		if((!cpu_irq_critical_section_counter) &&
		     cpu_irq_prev_interrupt_state) {
			__DMB();
			__enable_irq();
		}
	}
}

bool dmaAllocated = false;

void allocate()
{
    static const unint8_t channel = 0;
    cpu_irq_enter_critical();

    if (!dmaAllocated) {
    	PM->AHBMASK.bit.DMAC_       = 1; // Initialize DMA clocks
		PM->APBBMASK.bit.DMAC_      = 1;
		DMAC->CTRL.bit.DMAENABLE    = 0; // Disable DMA controller
		DMAC->CTRL.bit.SWRST        = 1; // Perform software reset

		// Initialize descriptor list addresses
		DMAC->BASEADDR.bit.BASEADDR = (uint32_t)_descriptor;
		DMAC->WRBADDR.bit.WRBADDR   = (uint32_t)_writeback;
		memset(_descriptor, 0, sizeof(_descriptor));
		memset(_writeback , 0, sizeof(_writeback));

		// Re-enable DMA controller with all priority levels
		DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xF);

		// Enable DMA interrupt at lowest priority
		NVIC_EnableIRQ(DMAC_IRQn);
		NVIC_SetPriority(DMAC_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
        dmaAllocated = true;
	}
	// Reset the allocated channel
	DMAC->CHID.bit.ID         = channel;
	DMAC->CHCTRLA.bit.ENABLE  = 0;
	DMAC->CHCTRLA.bit.SWRST   = 1;

	// Clear software trigger
	DMAC->SWTRIGCTRL.reg     &= ~(1 << channel);

	// Configure default behaviors
	DMAC->CHCTRLB.bit.LVL     = 0;
	DMAC->CHCTRLB.bit.TRIGSRC = peripheralTrigger;
	DMAC->CHCTRLB.bit.TRIGACT = triggerAction;

	cpu_irq_leave_critical();
}

// the setup routine runs once when you press reset:
void setup() {                
    // initialize the digital pin as an output.
    pinMode(led, OUTPUT);     
    Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);               // wait for a second
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    delay(200);               // wait for a second
    Serial.println("foo");
}
#endif