using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OpenHardwareMonitor.Hardware;
using System.Threading;

namespace temperatureGetter
{
    class Program
    {
        private static string tempInfo = string.Empty;
        static Computer computer = new Computer();
        static Visitor visitor = new Visitor();

        static void Main(string[] args)
        {
            
            computer.Open();

            Thread myThread = new Thread(new ThreadStart(startGetCpuTemperatureFuncThread));
            myThread.Start();

        }

        private static void startGetCpuTemperatureFuncThread() {
            while (true) {
                float? toWrite = GetCpuTemperature();
                using (StreamWriter sw = new StreamWriter("test.txt", false, System.Text.Encoding.Default))
                {
                    sw.WriteLine(toWrite);
                }
                Thread.Sleep(2000);
            }
        }

        private static float? GetCpuTemperature()
        {           

            Program.computer.CPUEnabled = true;
            Program.computer.Accept(Program.visitor);

            Program.tempInfo = string.Empty;
            
            int indexOfCpuTempSensor = 0;
            for (int i = 0; i < computer.Hardware.Length; i++)
            {
                if (computer.Hardware[i].HardwareType == HardwareType.CPU)
                {                    
                    for (int j = 0; j < computer.Hardware[i].Sensors.Length; j++)
                    {
                        if (computer.Hardware[i].Sensors[j].SensorType == SensorType.Temperature)
                        {
                            indexOfCpuTempSensor++;                            
                        }
                    }                    
                }
            }

            float?[] temp = new float?[indexOfCpuTempSensor];
            int k = 0;

            for (int i = 0; i < computer.Hardware.Length; ++i)
            {
                if (computer.Hardware[i].HardwareType == HardwareType.CPU)
                {                    
                    for (int j = 0; j < computer.Hardware[i].Sensors.Length; j++)
                    {                        
                        if (computer.Hardware[i].Sensors[j].SensorType == SensorType.Temperature)
                        {
                            temp[k] = computer.Hardware[i].Sensors[j].Value;                          
                            k++;                                                
                        }
                    }
                }
            }

            float? result = 0;            
            for (int i = 0; i < indexOfCpuTempSensor; i++) {
                result += temp[i];                
            }

            result = result / (float)indexOfCpuTempSensor;

            return result;

        }
    }
}
