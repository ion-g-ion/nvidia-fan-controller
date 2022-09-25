#include <stdio.h>
#include <stdlib.h>
#include <nvml.h>

int main(){
    printf("Welcome to the FAN controller\n");
    // devices  
    nvmlReturn_t result;
    nvmlDevice_t device;
    
    unsigned int temp, temp_prev, val, val_prev = 255;
    int delta_t = 500; // in ms
    unsigned int tmax = 75;
    unsigned int tmin = 42;
    unsigned int fmin = 45;
    unsigned int fmax = 255;
    char tmp[128];
    const char *fan_file = "/sys/class/hwmon/hwmon2/pwm2";

    int relaxation, max_relax = 6;

    // put the controls on manual by setting the flag on 1
    sprintf(tmp,"echo 1 >%s_enable",fan_file);
    while(system(tmp)){
        printf("Cannot set mode to manual. Repeating...\n");
        usleep(500000);
    }
    printf("Fan control set to manual\n");

    // put the fans to max
    sprintf(tmp,"echo %d >%s",val_prev, fan_file);
    system(tmp);

    result = nvmlInit();
    
    if(NVML_SUCCESS != result){
        printf("Failed initialization \n");
        return 1;
    }
    
    result = nvmlDeviceGetHandleByIndex(1, &device);
    
    relaxation = 0; // do not decrease the pwm immediatly when the temperature decreases
    while(1){
        result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
        if (NVML_SUCCESS != result) {
            printf("Failed to get device temperature: %s\n", nvmlErrorString(result));
            return 1;
        }
        else{
                //printf("temp %d\n",temp);
            if(temp>temp_prev || relaxation >= max_relax ){
                    relaxation = 0;
                    if(temp>tmax)
                        val = fmax;
                    else
                        if(temp<tmin)
                            val=fmin;
                        else
                            val = fmin+(temp-tmin)*(fmax-fmin)/(tmax-tmin);

                    if (val!=val_prev){
                        //printf("val %d\n",val);
                        sprintf(tmp,"echo %d >%s",val,fan_file);
                        val_prev = val;
                        system(tmp);
                    }
                    temp_prev = temp;
            }
            else{
                    ++relaxation;
            }
        }
        usleep(delta_t * 1000);

    }
    return 0;
}
