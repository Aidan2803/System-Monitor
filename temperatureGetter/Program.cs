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

            Thread cpuThread = new Thread(new ThreadStart(startGetCpuTemperatureFuncThread));
            cpuThread.Start();
            Thread gpuThread = new Thread(new ThreadStart(startGetGPUTemperatureFuncThread));
            gpuThread.Start();
            Thread hddThread = new Thread(new ThreadStart(startGetHDDTemperatureFuncThread));
            hddThread.Start();

        }

        private static void startGetCpuTemperatureFuncThread() {
            while (true) {
                float? toWrite = GetCpuTemperature();
                using (StreamWriter sw = new StreamWriter("temperatures.txt", false, System.Text.Encoding.Default))
                {
                    sw.WriteLine(toWrite);
                }
                Thread.Sleep(2000);
            }
        }

        private static void startGetGPUTemperatureFuncThread()
        {
            while (true)
            {
                float? toWrite = GetGPUTemperature();
                Thread.Sleep(50);
                using (StreamWriter sw = new StreamWriter("temperatures.txt", true, System.Text.Encoding.Default))
                {
                    sw.WriteLine(toWrite);
                }
                Thread.Sleep(1950);
            }
        }

        private static void startGetHDDTemperatureFuncThread()
        {
            while (true)
            {
                HDDtemperatures getRetrnedHDDTemps = new HDDtemperatures();
                getRetrnedHDDTemps = GetHDDTemperature();
                Thread.Sleep(100);
                using (StreamWriter sw = new StreamWriter("temperatures.txt", true, System.Text.Encoding.Default))
                {
                    for (int i = 0; i < 4; i++) {
                        if (getRetrnedHDDTemps.HDDtemp[i] != 0) {
                            sw.WriteLine(getRetrnedHDDTemps.HDDtemp[i]);
                        }
                    }                    
                }
                Thread.Sleep(1900);
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

        private static float? GetGPUTemperature(){            
            Program.computer.GPUEnabled = true;
            Program.computer.Accept(Program.visitor);

            Program.tempInfo = string.Empty;

            float? result = 0;

            for (int i = 0; i < computer.Hardware.Length; i++)
            {                
                if (computer.Hardware[i].HardwareType == HardwareType.GpuAti || computer.Hardware[i].HardwareType == HardwareType.GpuNvidia)
                {                    
                    for (int j = 0; j < computer.Hardware[i].Sensors.Length; j++)
                    {                        
                        if (computer.Hardware[i].Sensors[j].SensorType == SensorType.Temperature)
                        {
                            result = computer.Hardware[i].Sensors[j].Value;
                        }
                    }
                }
            }
            return result;
        }

        private static HDDtemperatures GetHDDTemperature()
        {
            Program.computer.HDDEnabled = true;
            Program.computer.Accept(Program.visitor);
            
            Program.tempInfo = string.Empty;            

            HDDtemperatures returnHDDtemp = new HDDtemperatures();

            int k = 0;
            for (int i = 0; i < computer.Hardware.Length; i++)
            {
                if (computer.Hardware[i].HardwareType == HardwareType.HDD)
                {                    
                    for (int j = 0; j < computer.Hardware[i].Sensors.Length; j++)
                    {                        
                        if (computer.Hardware[i].Sensors[j].SensorType == SensorType.Temperature)
                        {                            
                            returnHDDtemp.HDDtemp[k] = computer.Hardware[i].Sensors[j].Value;                            
                            k++;
                        }
                    }
                }
            }
            for (int i = k; i < 4; i++) {
                returnHDDtemp.HDDtemp[i] = 0;
            }

            return returnHDDtemp;
        }
    }
}
