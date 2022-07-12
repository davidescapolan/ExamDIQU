using ITS.DIQU.ScapolanDavide.Esame.Models;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using Microsoft.Azure.Devices;

namespace ITS.DIQU.ScapolanDavide.Esame.Pages
{
    public class IndexModel : PageModel
    {
        private readonly ILogger<IndexModel> _logger;

        static string _connectionString;
        static string _targetDevice1;
        static string _targetDevice2;
        static string _targetDevice3;

        private readonly RegistryManager registry;


        [BindProperty]
        public TConfigDeviceTwin Input { get; set; }
        public TDevice DeviceObject { get; set; }


        public IndexModel(IConfiguration configuration, ILogger<IndexModel> logger)
        {
            _logger = logger;

            _connectionString = configuration.GetConnectionString("IotHubConnectionString");
            _targetDevice1 = configuration.GetConnectionString("DeviceId1");
            _targetDevice2 = configuration.GetConnectionString("DeviceId2");
            _targetDevice3 = configuration.GetConnectionString("DeviceId3");

            registry = RegistryManager.CreateFromConnectionString(_connectionString);


            Input = new TConfigDeviceTwin();
            DeviceObject = new TDevice();
        }

        public async Task<IActionResult> OnGetAsync(string device = "device1")
        {

            //Set the comunication with the device1
            var deviceTwin = await registry.GetTwinAsync(device);

            //check if the Desired parameters contains Value
            if (deviceTwin.Properties.Desired.Contains("Micro1"))
            {
                //Insert the old value in the table
                var desiredValue = deviceTwin.Properties.Desired["Micro1"];
                DeviceObject.Micro1 = desiredValue;
            }
            if (deviceTwin.Properties.Desired.Contains("Micro2"))
            {
                //Insert the old value in the table
                var desiredValue = deviceTwin.Properties.Desired["Micro2"];
                DeviceObject.Micro2 = desiredValue;
            }
            if (deviceTwin.Properties.Desired.Contains("Micro3"))
            {
                //Insert the old value in the table
                var desiredValue = deviceTwin.Properties.Desired["Micro3"];
                DeviceObject.Micro3 = desiredValue;
            }

            return Page();
        }

        public async Task<IActionResult> OnPost()
        {
            //Get the value of the IdDevice field
            string idDevice = Input.IdDevice;
            Microsoft.Azure.Devices.Shared.Twin deviceTwin;

            switch (idDevice)
            {
                case "device2":
                    //Set the comunication with the Device
                    deviceTwin = await registry.GetTwinAsync(_targetDevice2);
                    break;
                case "device3":
                    //Set the comunication with the Device
                    deviceTwin = await registry.GetTwinAsync(_targetDevice3);
                    break;
                default:
                    //Set the comunication with the Device
                    deviceTwin = await registry.GetTwinAsync(_targetDevice1);
                    break;
            }

            //Get the value of the field
            int val = Input.Value;
            int micro = Input.Micro;
            if (val>9)
            {
                //Set the Desired properties
                deviceTwin.Properties.Desired[$"Micro{micro}"] = val.ToString();
            }
            else
            {
                //Set the Desired properties
                deviceTwin.Properties.Desired[$"Micro{micro}"] = '0' + val.ToString();
            }

            //Send to the Device the new configuration
            await registry.UpdateTwinAsync(idDevice, deviceTwin, deviceTwin.ETag);

            return Page();
        }
    }
}