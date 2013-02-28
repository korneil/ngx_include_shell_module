Include the output of program as nginx config.

**Install:**

./configure ... --add-module=[path_to_module_dir]

**Usage:**

Put include_shell directive to nginx.conf:

include_shell [path_to_file_where_the_output_will_be_generated] [program_to_execute];

**Example:**

include_shell /tmp/tmp-nginx.conf "/bin/echo root /var/www\;";

include_shell /tmp/tmp-nginx-gen.conf "/bin/php script_to_generate_conf.php";
