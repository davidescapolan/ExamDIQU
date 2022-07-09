using ITS.DIQU.ScapolanDavide.Esame.Models;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.RazorPages;
using Microsoft.Azure.Devices;

namespace ITS.DIQU.ScapolanDavide.Esame.Pages
{
    public class IndexModel : PageModel
    {
        private readonly ILogger<IndexModel> _logger;

        static ServiceClient serviceClient;
        static string _connectionString;
        static string _targetDevice1;
        static string _targetDevice2;
        static string _targetDevice3;

        private readonly RegistryManager registry;

        public IndexModel(IConfiguration configuration, ILogger<IndexModel> logger)
        {
            _logger = logger;

            _connectionString = configuration.GetConnectionString("IotHubConnectionString");
            _targetDevice1 = configuration.GetConnectionString("DeviceId1");
            _targetDevice2 = configuration.GetConnectionString("DeviceId2");
            _targetDevice3 = configuration.GetConnectionString("DeviceId3");


            registry = RegistryManager.CreateFromConnectionString(_connectionString);
        }

        [BindProperty]
        public TConfigDeviceTwin Input { get; set; }

        public async Task<IActionResult> OnGetAsync()
        {
            Input = new TConfigDeviceTwin();

            //Set the comunication with the device1
            var deviceTwin = await registry.GetTwinAsync(_targetDevice1);
            /*
            //check if the Reported parameters contains TimeBetween value
            if (deviceTwin.Properties.Reported.Contains("TimeBetween"))
            {
                //Insert the new old value in the input field
                var reportedTimeBetween = deviceTwin.Properties.Reported["TimeBetween"];
                Input.TimeBetween = reportedTimeBetween;
            }
            */
            return Page();
        }

        public async Task<IActionResult> OnPost()
        {
            //Get the value of the IdDevice field
            string idDevice = ""/*Input.IdDevice*/;

            //Set the comunication with the Device
            var deviceTwin = await registry.GetTwinAsync(idDevice);

            //Get the value of the TimeBetween field
            int timeBetween = 0/*Input.TimeBetween*/;

            //Set the Desired properties TimeBetween
            deviceTwin.Properties.Desired["TimeBetween"] = timeBetween;

            //Send to the Device the new configuration
            await registry.UpdateTwinAsync(idDevice, deviceTwin, deviceTwin.ETag);

            return Page();
        }
    }
}