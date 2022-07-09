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

        public IndexModel(IConfiguration configuration, ILogger<IndexModel> logger)
        {
            _logger = logger;

            _connectionString = configuration.GetConnectionString("IotHubConnectionString");
            _targetDevice1 = configuration.GetConnectionString("DeviceId1");
            _targetDevice2 = configuration.GetConnectionString("DeviceId2");
            _targetDevice3 = configuration.GetConnectionString("DeviceId3");
        }

        public void OnGet()
        {

        }
    }
}