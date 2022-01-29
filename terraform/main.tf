// Config
terraform {
  required_providers {
    yandex = {
      source  = "yandex-cloud/yandex"
      version = "0.70.0"
    }
  }
}

provider "yandex" {
  token     = ""
  cloud_id  = ""
  folder_id = ""
  zone      = "ru-central1-a"
}

// Disks
resource "yandex_compute_disk" "ssd-1" {
  name = "super-fast-ssd"
  size = 15
  type = "network-ssd"
}

// VMs
resource "yandex_compute_instance" "vm-1" {
  name = "terraform1"
  resources {
    cores  = 2
    memory = 2
  }
  boot_disk {
    initialize_params {
      image_id = "fd8qqvji2rs2lehr7d1l" // ubuntu-20-04-lts-v20211013
      size     = 50
      type     = "network-hdd"
    }
  }
  secondary_disk {
    disk_id = yandex_compute_disk.ssd-1.id
  }
  network_interface {
    subnet_id = yandex_vpc_subnet.subnet-1.id
    nat       = true
  }
  metadata = {
    user-data = "${file("./meta.txt")}"
  }
}
resource "yandex_compute_instance" "vm-2" {
  name = "terraform2"
  resources {
    cores  = 4
    memory = 4
  }
  boot_disk {
    initialize_params {
      image_id = "fd8qqvji2rs2lehr7d1l" // ubuntu-20-04-lts-v20211013
      size     = 93
      type     = "network-ssd-nonreplicated"
    }
  }
  network_interface {
    subnet_id = yandex_vpc_subnet.subnet-1.id
    nat       = true
  }
  metadata = {
    user-data = "${file("./meta.txt")}"
  }
}

// Network
resource "yandex_vpc_network" "network-1" {
  name = "network1"
}
resource "yandex_vpc_subnet" "subnet-1" {
  name           = "subnet1"
  zone           = "ru-central1-a"
  network_id     = yandex_vpc_network.network-1.id
  v4_cidr_blocks = ["192.168.10.0/24"]
}

// Internal and external IPs
output "internal_ip_address_vm_1" {
  value = yandex_compute_instance.vm-1.network_interface.0.ip_address
}
output "internal_ip_address_vm_2" {
  value = yandex_compute_instance.vm-2.network_interface.0.ip_address
}
output "external_ip_address_vm_1" {
  value = yandex_compute_instance.vm-1.network_interface.0.nat_ip_address
}
output "external_ip_address_vm_2" {
  value = yandex_compute_instance.vm-2.network_interface.0.nat_ip_address
}
