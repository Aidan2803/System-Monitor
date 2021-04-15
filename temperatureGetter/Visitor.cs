using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OpenHardwareMonitor.Hardware;

namespace temperatureGetter
{
    class Visitor : IVisitor
    {
        public void VisitComputer(IComputer computer)
        {
            computer.Traverse(this);
        }

        public void VisitHardware(IHardware hardware)
        {
            hardware.Update();

            foreach (IHardware hw in hardware.SubHardware)
            {
                hw.Accept(this);
            }
        }

        public void VisitParameter(IParameter parameter)
        {
        }

        public void VisitSensor(ISensor sensor)
        {
        }
    }
}
