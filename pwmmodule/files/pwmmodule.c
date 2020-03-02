#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/uaccess.h>          // Required for the copy to user function
//#include <unistd.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ismael Garcia");
MODULE_DESCRIPTION("PWM Module");

#define REGBASE 0x40000000


#define CONFIG_PWM 0X00
#define CONFIG_PERIOD 0X04
#define CONFIG_CH0 0X08
#define CONFIG_CH1 0X0C
#define CONFIG_CH2 0X10
#define CONFIG_CH3 0X14

#define CLASS_NAME "CLASS_PWM1"

#define SZ_4K sizeof(int)*4000
/*
/dev/pwm0/

/dev/pwm0/config
/dev/pwm0/channel_select
/dev/pwm0/period
/dev/pwm0/duty_period

*/

char kernel_buff[100];
static dev_t first;
static int    numberOpen = 0;


static int device_release(struct inode *inodep, struct file *filep);
static int device_open(struct inode *inodep, struct file *filep);
static ssize_t device_read(struct file *filep, char *buff, size_t len, loff_t *off);
static ssize_t device_write(struct file *filep, const char *buff,size_t len,loff_t *off);

static ssize_t get_configure_pwm(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t get_period_pwm(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t get_duty_channel0(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t get_duty_channel1(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t get_duty_channel2(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t get_duty_channel3(struct device *dev, struct device_attribute *attr, char *buf);


static ssize_t configure_pwm(struct device *dev, struct device_attribute *attr,const char *buf,size_t count);
static ssize_t period_pwm(struct device *dev, struct device_attribute *attr,const char *buf,size_t count);
static ssize_t duty_channel0(struct device *dev, struct device_attribute *attr,const char *buf,size_t count);
static ssize_t duty_channel1(struct device *dev, struct device_attribute *attr,const char *buf,size_t count);
static ssize_t duty_channel2(struct device *dev, struct device_attribute *attr,const char *buf,size_t count);
static ssize_t duty_channel3(struct device *dev, struct device_attribute *attr,const char *buf,size_t count);

static struct  class *device_class = NULL;
static struct device *this_device;
static struct cdev char_dev;
uint32_t reg_data;
//long configure_pwm_reg;
long  configure_pwm_reg;
long long   configure_pwm_period;
long  configure_pwm_ch0;
long  configure_pwm_ch1;
long  configure_pwm_ch2;
long  configure_pwm_ch3;


static struct file_operations fileops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static DEVICE_ATTR(configure,0644,get_configure_pwm, configure_pwm);
//static DEVICE_ATTR(configure,S_IRUGO| S_IRUGO,get_configure_pwm,NULL);

static DEVICE_ATTR(period,0644,get_period_pwm, period_pwm);
static DEVICE_ATTR(duty_ch0,0644,get_duty_channel0, duty_channel0);
static DEVICE_ATTR(duty_ch1,0644,get_duty_channel1, duty_channel1);
static DEVICE_ATTR(duty_ch2,0644,get_duty_channel2, duty_channel2);
static DEVICE_ATTR(duty_ch3,0644,get_duty_channel3, duty_channel3);
/*
static DEVICE_ATTR(period,S_IRUGO| S_IRUGO,get_period_pwm, period_pwm);
static DEVICE_ATTR(duty_ch0,S_IRUGO| S_IRUGO,get_duty_channel0, duty_channel0);
static DEVICE_ATTR(duty_ch1,S_IRUGO| S_IRUGO,get_duty_channel1, duty_channel1);
static DEVICE_ATTR(duty_ch2,S_IRUGO| S_IRUGO,get_duty_channel2, duty_channel2);
static DEVICE_ATTR(duty_ch3,S_IRUGO| S_IRUGO,get_duty_channel3, duty_channel3);
*/

/*static struct bus_attribute dev_attr_parCrtl = {
  .attr.name = "configurePWM"
  .attr.mode = S_IWUSR,
  .show = NULL,
  .store = configure_pwm
}
*/

//    static int device_file_major_number = 0;
//static const char device_name[] = "pmw0-driver";

/*static int register_device(void)
{
        int result = 0;
        printk( KERN_NOTICE "Simple-driver: register_device() is called." );
        result = register_chrdev( 0, device_name, &fileops );
        if( result < 0 )
        {
            printk( KERN_WARNING "Simple-driver:  can\'t register character device with errorcode = %i", result );
            return result;
        }
        device_file_major_number = result;
        printk( KERN_NOTICE "Simple-driver: registered character device with major number = %i and minor numbers 0...255"
             , device_file_major_number );
        return 0;
}*/


 

static ssize_t get_configure_pwm(struct device *dev, struct device_attribute *attr,   char *buf){
  printk(KERN_INFO "read from configure_pwm attribute: %ld.\n",configure_pwm_reg);  
   
  //sprintf(buf,"        %ld",configure_pwm_reg);
  sprintf(buf,  "%ld\n",configure_pwm_reg);

  return strlen(buf) + 1;

  //iowrite32(io,reg_data);
}

static ssize_t get_period_pwm(struct device *dev, struct device_attribute *attr,   char *buf){
  printk(KERN_INFO "read from period_pwm attribute: %lld.\n",configure_pwm_period);  

  sprintf(buf,"%lld\n",configure_pwm_period);

  return strlen(buf) + 1;
}
 
static ssize_t get_duty_channel0(struct device *dev, struct device_attribute *attr, char *buf){
  printk(KERN_ALERT "read from duty_channel0 attribute: %ld.\n",configure_pwm_ch0);

  sprintf(buf, "%ld\n",configure_pwm_ch0);

  return strlen(buf) + 1;
}

static ssize_t get_duty_channel1(struct device *dev, struct device_attribute *attr, char *buf){
  printk(KERN_ALERT "read from duty_channel1 attribute: %ld.\n",configure_pwm_ch1);

  sprintf(buf,  "        %ld\n",configure_pwm_ch1);

  return strlen(buf) + 1;
}

static ssize_t get_duty_channel2(struct device *dev, struct device_attribute *attr, char *buf){
  printk(KERN_ALERT "read from duty_channel2 attribute: %ld.\n",configure_pwm_ch2);

  sprintf(buf,  "        %ld\n",configure_pwm_ch2);

  return strlen(buf) + 1;
}

static ssize_t get_duty_channel3(struct device *dev, struct device_attribute *attr, char *buf){
  printk(KERN_ALERT "read from duty_channel3 attribute: %ld.\n",configure_pwm_ch3);

  sprintf(buf, "%ld\n",configure_pwm_ch3);

  return strlen(buf) + 1;
}

static ssize_t configure_pwm(struct device *dev, struct device_attribute *attr, const char *buf,size_t count){
  printk(KERN_INFO "write to configure_pwm attribute: %s : count %ld.\n", buf, count);  
  void __iomem *io; 
  io = ioremap(REGBASE + CONFIG_PWM, SZ_4K);
 
  if (kstrtol(buf,0, &configure_pwm_reg) == 0){
 
    printk(KERN_INFO "Successfully store configure_pwm data: %ld\n",configure_pwm_reg);  
  } 
  
   iowrite32(configure_pwm_reg & 0x1F,io);

  iounmap(io);
 return count;
}
 

static ssize_t period_pwm(struct device *dev, struct device_attribute *attr,const char *buf,size_t count){
  printk(KERN_INFO "write to period_pwm attribute.\n");

  void __iomem *io; 
  io = ioremap(REGBASE + CONFIG_PERIOD, SZ_4K);
 
  if (kstrtoll (buf,0, &configure_pwm_period) == 0){
    printk(KERN_INFO "Successfully store period_pwm data: %lld\n",configure_pwm_period);  
  } 
  iowrite32(configure_pwm_period,io);
  iounmap(io);
  return count;
}

 
static ssize_t duty_channel0(struct device *dev, struct device_attribute *attr, const char *buf,size_t count){
  printk(KERN_INFO "write to duty_channel0 attribute.\n");
 
  void __iomem *io; 
  io = ioremap(REGBASE + CONFIG_CH0, SZ_4K);
 
  if (kstrtol(buf,0, &configure_pwm_ch0) == 0){
 
    printk(KERN_INFO "Successfully store duty_channel0 data: %ld\n",configure_pwm_ch0);  
  } 
 
  iowrite32(configure_pwm_ch0,io);
  iounmap(io);

return count;
}

static ssize_t duty_channel1(struct device *dev, struct device_attribute *attr, const char *buf,size_t count){
  printk(KERN_INFO "write to duty_channel1 attribute.\n");
  void __iomem *io; 
  io = ioremap(REGBASE + CONFIG_CH1, SZ_4K);
 
  if (kstrtol(buf,0, &configure_pwm_ch1) == 0){
 
    printk(KERN_INFO "Successfully store duty_channel1 data: %ld\n",configure_pwm_ch1);  
  } 
  iowrite32(configure_pwm_ch1,io);
 iounmap(io);
return count;
}

static ssize_t duty_channel2(struct device *dev, struct device_attribute *attr, const char *buf,size_t count){
  void __iomem *io; 
  io = ioremap(REGBASE + CONFIG_CH2, SZ_4K);
 
  if (kstrtol(buf,0, &configure_pwm_ch2) == 0){
    printk(KERN_INFO "Successfully store duty_channel2 data: %ld\n",configure_pwm_ch2);  
  } 

  iowrite32(configure_pwm_ch2,io);
 iounmap(io);
return count;
}

static ssize_t duty_channel3(struct device *dev, struct device_attribute *attr,const char *buf,size_t count){


 void __iomem *io; 
  io = ioremap(REGBASE + CONFIG_CH3, SZ_4K);
 
  if (kstrtol(buf,0, &configure_pwm_ch3) == 0){
 
    printk(KERN_INFO "Successfully store duty_channel3 data: %ld\n",configure_pwm_ch3);  
  } 
  iowrite32(configure_pwm_ch3,io);
 iounmap(io);
return count;
}

static int device_open(struct inode *inodep, struct file *filep){
  numberOpen++;
   printk(KERN_INFO "EBBChar: Device has been opened %d time(s)\n",numberOpen);
   return 0;
}

static ssize_t device_read(struct file *filep, char *buff, size_t len, loff_t *off){
   long data =   sprintf(kernel_buff,"%ld",configure_pwm_reg);
    copy_to_user(buff, kernel_buff, data);
return 0;
}

static ssize_t device_write(struct file *filep, const char *buff,size_t len,loff_t *off){
	//copy_from_user(ker_buf,buff,len);
	printk(KERN_INFO "Write to file.\n");
	//void __iomem *io;
	//sscanf(kernel_buff,"%l",buff,len);

	return len;
}

static int device_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "EBBChar: Device successfully closed\n");
   return 0;
}

static int __init pwm_init(void)
{
    printk(KERN_INFO "Initialize pwm0 kernel module!\n");

	if(alloc_chrdev_region(&first,0,1,"ismael") < 0){
	return -1;
	}


	device_class = class_create(THIS_MODULE,CLASS_NAME);
	if(device_class == NULL){
		printk(KERN_INFO "failed to create class\n");
	 	unregister_chrdev_region(first,1);
		return -1;
	}
		this_device = device_create(device_class,NULL,first,NULL,"pwm0");
	
            if(device_class == NULL){
		printk(KERN_INFO "failed to create device\n");
		class_destroy(device_class);
	 	unregister_chrdev_region(first,1);
		return -1;
	}	
		cdev_init(&char_dev,&fileops);
		
	if(cdev_add(&char_dev,first,1) == -1){
		printk(KERN_INFO "failed to add char device\n");
		device_destroy(device_class,first);
		class_destroy(device_class);
		unregister_chrdev_region(first,1);
		
			
		 return -1;	
		}
		
	if(device_create_file(this_device,&dev_attr_configure) < 0){
    	
	
	}
	
	if(device_create_file(this_device,&dev_attr_period) < 0){
    	
	
	}	
	if(device_create_file(this_device,&dev_attr_duty_ch0) < 0){
    	
	
	}	
	if(device_create_file(this_device,&dev_attr_duty_ch1) < 0){
    	
	
	}
	if(device_create_file(this_device,&dev_attr_duty_ch2) < 0){
    	
	
	}
	if(device_create_file(this_device,&dev_attr_duty_ch3) < 0){
    	
	
	}
	
	printk(KERN_INFO "Done initializing:  %s\n",CLASS_NAME);
	
	
    return 0;    
}





static void __exit pwm_exit(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
	cdev_del(&char_dev);
	device_remove_file(this_device,&dev_attr_configure);
	device_remove_file(this_device,&dev_attr_period);
	device_remove_file(this_device,&dev_attr_duty_ch0);
	device_remove_file(this_device,&dev_attr_duty_ch1);
	device_remove_file(this_device,&dev_attr_duty_ch2);
	device_remove_file(this_device,&dev_attr_duty_ch3);
	device_destroy(device_class,first);
	class_destroy(device_class);
}

module_init(pwm_init);
module_exit(pwm_exit);
