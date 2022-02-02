terraform {
  required_providers {
    yandex = {
      source  = "yandex-cloud/yandex"
      version = "0.70.0"
    }
  }
}
provider "yandex" {
  token     = var.token
  cloud_id  = var.cloud_id
  folder_id = var.folder_id
  zone      = var.zone
}

// Network and subnets
resource "yandex_vpc_network" "main-network" {
  name = "main-network"
}
resource "yandex_vpc_subnet" "public-subnet" {
  name           = "public-subnet"
  zone           = "ru-central1-a"
  network_id     = yandex_vpc_network.main-network.id
  v4_cidr_blocks = ["192.168.10.0/24"]
}
resource "yandex_vpc_subnet" "private-subnet" {
  name           = "private-subnet"
  zone           = "ru-central1-a"
  network_id     = yandex_vpc_network.main-network.id
  v4_cidr_blocks = ["192.168.11.0/24"]

  // binding routing table for redirecting traffic to nat instance
  route_table_id = yandex_vpc_route_table.nat-instance-route.id
}

// Route table for NAT instance
resource "yandex_vpc_route_table" "nat-instance-route" {
  network_id = yandex_vpc_network.main-network.id

  static_route {
    destination_prefix = "0.0.0.0/0"
    next_hop_address   = yandex_compute_instance.nat-instance.network_interface.0.ip_address
  }
}

// Nework Load Balancer
resource "yandex_lb_network_load_balancer" "logbroker-balancer" {
  name = "logbroker-network-load-balancer"

  listener {
    name = "main-listener"
    port = 80
    external_address_spec {
      ip_version = "ipv4"
    }
  }

  attached_target_group {
    target_group_id = yandex_lb_target_group.logbroker-target-group.id

    healthcheck {
      name = "healthcheck-backends-and-clickhouse-too"
      interval = 10
      timeout = 5
      http_options {
        port = 80
        path = "/healthcheck"
      }
    }
  }
}

// VMs
resource "yandex_compute_instance" "clickhouse" {
  name = "clickhouse"
  hostname = "clickhouse"
  resources {
    cores  = 8
    memory = 8
  }
  boot_disk {
    initialize_params {
      image_id = "fd8qqvji2rs2lehr7d1l" // ubuntu-20-04-lts-v20211013
      size     = 100
      type     = "network-hdd"
    }
  }
  network_interface {
    subnet_id = yandex_vpc_subnet.private-subnet.id
    nat       = false
  }
  metadata = {
    user-data = "${file("./meta.txt")}"
  }
}

resource "yandex_compute_instance" "nat-instance" {
  name = "nat-instance"
  hostname = "nat-instance"
  resources {
    cores  = 2
    memory = 1
    core_fraction = 20
  }
  boot_disk {
    initialize_params {
      image_id = "fd8drj7lsj7btotd7et5" // nat-instance-ubuntu
    }
  }
  network_interface {
    subnet_id = yandex_vpc_subnet.public-subnet.id
    nat       = true
  }
  metadata = {
    user-data = "${file("./meta.txt")}"
  }
}

resource "yandex_compute_instance" "logbroker" {
  count = 5
  name = "logbroker-${count.index + 1}"
  hostname = "logbroker-${count.index + 1}"
  resources {
    cores  = 4
    memory = 4
  }
  boot_disk {
    initialize_params {
      image_id = "fd8qqvji2rs2lehr7d1l" // ubuntu-20-04-lts-v20211013
      size     = 25
      type     = "network-hdd"
    }
  }
  network_interface {
    subnet_id = yandex_vpc_subnet.private-subnet.id
    nat       = false
  }
  metadata = {
    user-data = "${file("./meta.txt")}"
  }
}

locals {
  ips = toset(yandex_compute_instance.logbroker[*].network_interface.0.ip_address)
} 
resource "yandex_lb_target_group" "logbroker-target-group" {
  name      = "logbroker-target-group"
  region_id = "ru-central1"
  // dirty hax tho
  dynamic "target" {
    for_each = local.ips
    content {
      subnet_id = yandex_vpc_subnet.private-subnet.id
      address   = target.value
    }
  }
}

output "internal_ip_address_clickhouse" {
  value = yandex_compute_instance.clickhouse.network_interface.0.ip_address
}
output "internal_ip_addresses_logbroker" {
    value = yandex_compute_instance.logbroker[*].network_interface.0.ip_address
}
output "external_ip_address_nat-instance" {
    value = yandex_compute_instance.nat-instance.network_interface.0.nat_ip_address
}
output "external_ip_balancer" {
    value = "${element(yandex_lb_network_load_balancer.logbroker-balancer.listener[*].external_address_spec, 0)}"
}
