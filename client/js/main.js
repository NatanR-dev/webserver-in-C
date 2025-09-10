const App = {
  elements: {

    settings: {
      form: document.querySelector('#account .card'),
      saveButton: document.getElementById('save-settings'),
      status: document.getElementById('settings-status')
    },
    
    overview: {
      container: document.getElementById('overview'),
      machinesList: document.getElementById('overview-machines'),
      activityChart: document.getElementById('overview-activity-chart'),
      machines: document.getElementById('overview-machines')
    },
    
    overviewMachines: document.getElementById('overview-machines'),

    machines: {
      container: document.getElementById('machines'),
      list: document.getElementById('machines-list'),
      specs: {
        container: document.getElementById('specs'),
        list: document.getElementById('specs-list')
      },
      system: {
        container: document.getElementById('system'),
        list: document.getElementById('system-list')
      }
    },
    
    activity: {
      container: document.getElementById('activity'),
      list: document.getElementById('activity-list'),
      timeline: {
        container: document.getElementById('login-timeline'),
        filter: document.getElementById('timeline-filter')
      }
    },
    
    logs: {
      container: document.getElementById('logs'),
      list: document.getElementById('logs-list'),
      filter: document.getElementById('logs-filter')
    },
    
    snapshots: {
      container: document.getElementById('snapshots'),
      list: document.getElementById('snapshots-list'),
      filter: document.getElementById('snapshots-filter')
    },
    
    // COMMON
    errorNote: document.getElementById('error-note'),
    loadingOverlay: document.querySelector('.loading-overlay'),
    overviewActivityChart: document.getElementById('overview-activity-chart'),
    powerModal: document.getElementById('power-modal'),
    modalTitle: document.getElementById('modal-title'),
    modalBoot: document.getElementById('modal-boot'),
    modalReboot: document.getElementById('modal-reboot'),
    modalShutdown: document.getElementById('modal-shutdown'),
    modalStatus: document.getElementById('modal-status'),
    modalClose: document.getElementById('modal-close'),
    terminalModal: document.getElementById('terminal-modal'),
    terminalTitle: document.getElementById('terminal-title'),
    terminalOutput: document.getElementById('terminal-output'),
    terminalInput: document.getElementById('terminal-input'),
    terminalMinimize: document.querySelector('.terminal-control.minimize'),
    terminalMaximize: document.querySelector('.terminal-control.maximize'),
    terminalClose: document.querySelector('.terminal-control.close'),
    terminalCursor: document.querySelector('.terminal-cursor'),
    sidebarItems: document.querySelectorAll('.sidebar .menu-item'),
    sidebarSubItems: document.querySelectorAll('.sidebar .submenu li'),
    contentSections: document.querySelectorAll('.content-section'),
    mainContent: document.querySelector('.main-content')
  },

  currentMachineId: null,
  minLoadingDuration: 1000,
  terminalHistory: {},
  terminalCommandIndex: {},
  isTerminalMaximized: false,
  machinesData: [],

  initialize() {
    console.log('Initializing application...');
    
    this.setupSidebar();
    this.setupLogoutButton();
    this.setupCursor();
    this.setupSettings();
    this.setupModals();
    
    setTimeout(() => {
      this.loadMachineData();
    }, 100);
  },

  setupLogoutButton() {
    const logoutBtn = document.getElementById('logout-btn');
    if (logoutBtn) {
      logoutBtn.addEventListener('click', (e) => {
        e.preventDefault();

        console.log('Logout clicked');

        alert('Logout successful!');
      });
    }
  },

  setupSidebar() {
    this.elements.sidebarItems.forEach(item => {
      item.addEventListener('click', (e) => {
        const sectionId = item.dataset.section;
        const toggleId = item.dataset.toggle;

        if (toggleId) {
          e.stopPropagation();
          const submenu = document.getElementById(toggleId);
          const isActive = submenu.classList.contains('active');

          document.querySelectorAll('.sidebar .submenu').forEach(sm => {
            sm.classList.remove('active');
            sm.parentElement.classList.remove('active');
          });

          if (!isActive) {
            submenu.classList.add('active');
            item.classList.add('active');
            console.log(`Submenu ${toggleId} expanded`);
          }
        } else if (sectionId) {
          
          this.elements.sidebarItems.forEach(i => i.classList.remove('active'));
          this.elements.sidebarSubItems.forEach(i => i.classList.remove('active'));
          item.classList.add('active');
          this.elements.contentSections.forEach(section => section.classList.remove('active'));
          document.getElementById(sectionId).classList.add('active');
          console.log(`Navigated to section: ${sectionId}`);
          this.handleSectionNavigation(sectionId);
        }
      });
    });

    this.elements.sidebarSubItems.forEach(subItem => {
      subItem.addEventListener('click', (e) => {
        e.stopPropagation();
        const sectionId = subItem.dataset.section;
        this.elements.sidebarItems.forEach(i => i.classList.remove('active'));
        this.elements.sidebarSubItems.forEach(i => i.classList.remove('active'));
        subItem.classList.add('active');
        subItem.parentElement.classList.add('active');
        subItem.parentElement.parentElement.classList.add('active');
        this.elements.contentSections.forEach(section => section.classList.remove('active'));
        document.getElementById(sectionId).classList.add('active');
        console.log(`Navigated to subsection: ${sectionId}`);
        this.handleSectionNavigation(sectionId);
      });
    });
  },

  handleSectionNavigation(sectionId) {
    if (sectionId === 'activity') {
      this.renderTimeline(this.machinesData);
    } else if (sectionId === 'logs') {
      this.renderLogs(this.machinesData);
    } else if (sectionId === 'snapshots') {
      this.renderSnapshots(this.machinesData);
    }
  },

  setupCursor() {
    const input = this.elements.terminalInput;
    const cursor = this.elements.terminalCursor;
    const container = document.querySelector('.terminal-input-container');
    const prompt = document.querySelector('.terminal-prompt');

    const textMeasure = document.createElement('span');
    textMeasure.className = 'text-measure';
    container.appendChild(textMeasure);

    const updateCursorPosition = () => {
      textMeasure.textContent = input.value;
      const promptWidth = prompt.getBoundingClientRect().width;
      const textWidth = textMeasure.getBoundingClientRect().width;
      cursor.style.left = `${promptWidth + textWidth + 10}px`;
      //cursor.style.top = `${(input.offsetHeight - 14) / 2}px`;
    };

    input.addEventListener('input', updateCursorPosition);
    input.addEventListener('keydown', updateCursorPosition);
    input.addEventListener('click', updateCursorPosition);
    input.addEventListener('focus', updateCursorPosition);

    updateCursorPosition();
  },

  setupSettings() {
    const form = document.querySelector('#account .card');
    const saveButton = document.getElementById('save-settings');
    const status = document.getElementById('settings-status');

    const mockSettings = {
      accountName: 'Lorem User',
      email: 'user@mail.com',
      region: 'sa-east-1',
      mfaEnabled: false
    };

    if (form) {
      form.querySelector('#account-name').value = mockSettings.accountName;
      form.querySelector('#email').value = mockSettings.email;
      form.querySelector('#region').value = mockSettings.region;
      form.querySelector('#mfa-enabled').checked = mockSettings.mfaEnabled;
    }

    if (saveButton) {
      saveButton.addEventListener('click', (e) => {
        e.preventDefault();
        
        const accountName = form.querySelector('#account-name').value.trim();
        const email = form.querySelector('#email').value.trim();
        const region = form.querySelector('#region').value;
        const mfaEnabled = form.querySelector('#mfa-enabled').checked;

        if (!accountName || !email) {
          this.showStatusMessage('Please fill in all required fields', 'error');
          return;
        }

        if (!/^\S+@\S+\.\S+$/.test(email)) {
          this.showStatusMessage('Please enter a valid email address', 'error');
          return;
        }

        console.log('Saving settings:', { accountName, email, region, mfaEnabled });
        this.showStatusMessage('Settings saved successfully!', 'success');
      });
    }

    const accountSection = document.getElementById('account');
    if (accountSection) {
      accountSection.classList.remove('active');
    }
  },
  
  showStatusMessage(message, type = 'info') {
    const status = document.getElementById('settings-status');
    if (status) {
      status.textContent = message;
      status.className = 'settings-status';
      status.classList.add(type);
      
      setTimeout(() => {
        status.textContent = '';
        status.className = 'settings-status';
      }, 3000);
    }
  },

  async loadMachineData() {
    const startTime = Date.now();
    try {
      const machinesData = await this.fetchData();
      this.machinesData = machinesData;
      console.log('Stored machinesData:', this.machinesData);
      const elapsedTime = Date.now() - startTime;
      const remainingTime = this.minLoadingDuration - elapsedTime;

      setTimeout(() => {
        this.renderOverview(machinesData);
        this.renderMachines(machinesData);
        this.renderSpecs(machinesData);
        this.renderSystem(machinesData);
        this.renderActivity(machinesData);
        this.renderLogs(machinesData);
        this.renderSnapshots(machinesData);
        this.hideLoadingOverlay();
      }, Math.max(0, remainingTime));
    } catch (error) {
      this.handleError(error, startTime);
    }
  },

  setupModals() {
    const closeModal = (modal) => {
      modal.style.display = 'none';
      document.body.style.overflow = 'auto';
    };

    this.elements.modalClose.addEventListener('click', () => {
      closeModal(this.elements.powerModal);
    });

    this.elements.terminalClose.addEventListener('click', () => {
      closeModal(this.elements.terminalModal);
    });

    this.elements.terminalMinimize.addEventListener('click', () => {
      if (this.elements.terminalModal.classList.contains('minimized')) {
        this.elements.terminalModal.classList.remove('minimized');
      } else {
        this.elements.terminalModal.classList.add('minimized');
      }
    });

    this.elements.terminalMaximize.addEventListener('click', () => {
      if (this.elements.terminalModal.classList.contains('maximized')) {
        this.elements.terminalModal.classList.remove('maximized');
        this.elements.terminalModal.style.width = '600px';
        this.elements.terminalModal.style.height = 'auto';
      } else {
        this.elements.terminalModal.classList.add('maximized');
        this.elements.terminalModal.style.width = '95vw';
        this.elements.terminalModal.style.height = '80vh';
      }
    });

    window.addEventListener('click', (event) => {
      if (event.target === this.elements.powerModal) {
        closeModal(this.elements.powerModal);
      }
      if (event.target === this.elements.terminalModal) {
        closeModal(this.elements.terminalModal);
      }
    });

    this.elements.modalBoot.addEventListener('click', () => this.handleAction('boot'));
    this.elements.modalReboot.addEventListener('click', () => this.handleAction('reboot'));
    this.elements.modalShutdown.addEventListener('click', () => this.handleAction('shutdown'));
  },

  async fetchData() {
    try {
      const [osResponse, sysResponse, machineResponse] = await Promise.all([
        fetch('http://localhost:8080/api/os'),
        fetch('http://localhost:8080/api/sys'),
        fetch('http://localhost:8080/api/machine')
      ]);

      const osStatus = osResponse.status;
      const sysStatus = sysResponse.status;
      const machineStatus = machineResponse.status;
      
      console.log(`API status - OS: ${osStatus}, Sys: ${sysStatus}, Machine: ${machineStatus}`);

      if (!osResponse.ok || !sysResponse.ok || !machineResponse.ok) {
        throw new Error(`API failed - OS: ${osStatus}, Sys: ${sysStatus}, Machine: ${machineStatus}`);
      }

      const osData = await osResponse.json();
      const sysData = await sysResponse.json();
      const machineData = await machineResponse.json();

      const osArray = Array.isArray(osData) ? osData : [osData];
      const sysArray = Array.isArray(sysData) ? sysData : [sysData];
      const machineArray = Array.isArray(machineData) ? machineData : [machineData];

      const machines = osArray.map((os, index) => {
        const machineInfo = machineArray[index] || machineArray[0] || {};
        const machine = {
          id: index + 1,
          os: os.os || 'Unknown',
          powerState: osStatus === 200 && sysStatus === 200 ? 'running' : 'stopped',
          lastLogin: machineInfo.date || os.lastLogin || '2025-09-04 16:53:00',
          ip: machineInfo.ip || os.ip || '192.168.1.100',
          location: os.location || 'Unknown',
          status: machineInfo.status || 'unknown',
          info: machineInfo.info || {},
          loginCount: os.loginCount || Math.floor(Math.random() * 10) + 1,
          arch: sysArray[index]?.arch || 'Unknown',
          processor: sysArray[index]?.processorArchitecture || 'Unknown',
          cores: sysArray[index]?.numberOfCores || 0,
          logicalProcessors: sysArray[index]?.numberOfLogicalProcessors || 0,
          loginHistory: [],
          logs: [],
          snapshots: []
        };
        
        machine.loginHistory = this.generateLoginHistory(machine.id, machine.lastLogin, machine.ip, machine.loginCount);
        machine.logs = this.generateLogs(machine.id, machine.lastLogin);
        machine.snapshots = this.generateSnapshots(machine.id, machine.lastLogin);
        
        console.log(`Machine ${machine.id} data:`, {
          ...machine,
          loginHistory: `[${machine.loginHistory.length} items]`,
          logs: `[${machine.logs.length} items]`,
          snapshots: `[${machine.snapshots.length} items]`
        });
        
        return machine;
      });

      console.log('All machines data:', machines);
      return machines;
    } catch (error) {
      console.error('Fetch error:', error);
      throw error;
    }
  },

  generateLoginHistory(machineId, lastLogin, ip, loginCount) {
    const history = [];
    const baseDate = lastLogin && !isNaN(Date.parse(lastLogin)) ? new Date(lastLogin) : new Date('2025-09-04T16:53:00');
    const ipBase = ip && ip !== 'Unknown' ? ip.split('.')[0] + '.' + ip.split('.')[1] : '192.168';
    const maxEvents = Math.min(loginCount * 2, 10);

    if (loginCount < 1) {
      console.warn(`No login history generated for Machine ${machineId}: loginCount is ${loginCount}`);
      return history;
    }

    for (let i = 0; i < maxEvents; i += 2) {
      const loginTime = new Date(baseDate.getTime() - (i * 24 * 60 * 60 * 1000) - (Math.random() * 12 * 60 * 60 * 1000));
      const isFailed = Math.random() < 0.2;
      const eventIp = `${ipBase}.${Math.floor(Math.random() * 10) + 1}.${Math.floor(Math.random() * 10) + 100}`;
      history.push({
        machineId,
        eventType: 'Login',
        status: isFailed ? 'Failed' : 'Successful',
        timestamp: loginTime.toISOString().replace('T', ' ').slice(0, 19),
        ip: eventIp,
        loginCount: loginCount - Math.floor(i / 2)
      });
      if (i < maxEvents - 1 && !isFailed) {
        const durationMs = Math.random() * 2 * 60 * 60 * 1000 + 60 * 1000;
        const logoutTime = new Date(loginTime.getTime() + durationMs);
        const durationStr = this.formatDuration(durationMs);
        history.push({
          machineId,
          eventType: 'Logout',
          status: 'Successful',
          timestamp: logoutTime.toISOString().replace('T', ' ').slice(0, 19),
          ip: eventIp,
          loginCount: loginCount - Math.floor(i / 2),
          sessionDuration: durationStr
        });
      }
    }

    const sortedHistory = history.sort((a, b) => new Date(b.timestamp) - new Date(a.timestamp));
    console.log(`Sorted loginHistory for Machine ${machineId}:`, sortedHistory);
    return sortedHistory;
  },

  generateLogs(machineId, lastLogin) {
    const logs = [];
    const baseDate = lastLogin && !isNaN(Date.parse(lastLogin)) ? new Date(lastLogin) : new Date('2025-09-04T16:53:00');
    const severities = ['INFO', 'WARNING', 'ERROR'];
    const messages = [
      'System started successfully',
      'High CPU usage detected',
      'Failed to connect to database',
      'User authentication successful',
      'Network latency warning',
      'Disk space low'
    ];

    for (let i = 0; i < 8; i++) {
      const logTime = new Date(baseDate.getTime() - (i * 2 * 60 * 60 * 1000));
      const severity = severities[Math.floor(Math.random() * severities.length)];
      const message = messages[Math.floor(Math.random() * messages.length)];
      logs.push({
        machineId,
        timestamp: logTime.toISOString().replace('T', ' ').slice(0, 19),
        severity,
        message
      });
    }

    const sortedLogs = logs.sort((a, b) => new Date(b.timestamp) - new Date(a.timestamp));
    console.log(`Sorted logs for Machine ${machineId}:`, sortedLogs);
    return sortedLogs;
  },

  generateSnapshots(machineId, lastLogin) {
    const snapshots = [];
    const baseDate = lastLogin && !isNaN(Date.parse(lastLogin)) ? new Date(lastLogin) : new Date('2025-09-04T16:53:00');
    const statuses = ['completed', 'pending', 'failed'];

    for (let i = 0; i < 5; i++) {
      const snapshotTime = new Date(baseDate.getTime() - (i * 24 * 60 * 60 * 1000));
      const status = statuses[Math.floor(Math.random() * statuses.length)];
      snapshots.push({
        machineId,
        snapshotId: `snap-${machineId}-${Math.random().toString(36).substring(2, 10)}`,
        creationTime: snapshotTime.toISOString().replace('T', ' ').slice(0, 19),
        size: `${Math.floor(Math.random() * 50) + 10} GB`,
        status
      });
    }

    const sortedSnapshots = snapshots.sort((a, b) => new Date(b.creationTime) - new Date(a.creationTime));
    console.log(`Sorted snapshots for Machine ${machineId}:`, sortedSnapshots);
    return sortedSnapshots;
  },

  formatDuration(ms) {
    const hours = Math.floor(ms / (1000 * 60 * 60));
    const minutes = Math.floor((ms % (1000 * 60 * 60)) / (1000 * 60));
    return `${hours > 0 ? hours + 'h ' : ''}${minutes}m`;
  },

  renderOverview(machines) {
    const machineHTML = machines.map(machine => {
      const statusClass = machine.status ? `status-${machine.status.toLowerCase()}` : '';
      const machineId = machine.info?.id || `machine-${machine.id}`;
      const lastActive = machine.info?.date || machine.lastLogin;
      
      return `
        <tr>
          <td>${machineId}</td>
          <td>${machine.os}</td>
          <td class="status-cell">
            <span class="status-badge ${statusClass}">${machine.status || 'unknown'}</span>
          </td>
          <td>${lastActive}</td>
          <td>${machine.ip}</td>
          <td>${machine.location}</td>
          <td>${machine.loginCount}</td>
          <td>
            <div class="action-buttons">
              <button class="action-button power-button" data-machine-id="${machine.id}" data-machine-status="${machine.status || 'unknown'}">Power</button>
              <button class="action-button terminal-button" data-machine-id="${machine.id}" ${machine.status !== 'started' ? 'disabled' : ''}>Terminal</button>
              <button class="action-button toggle-specs" data-machine-id="${machine.id}">Show Specs</button>
            </div>
          </td>
        </tr>
        <tr class="machine-specs" id="overview-specs-${machine.id}" style="display: none;">
          <td colspan="8">
            <table class="details-table">
              <tbody>
                ${this.createTableRow('Status', machine.status || 'unknown')}
                ${this.createTableRow('Architecture', machine.arch)}
                ${this.createTableRow('Processor', machine.processor)}
                ${this.createTableRow('Cores', machine.cores)}
                ${this.createTableRow('Logical Processors', machine.logicalProcessors)}
                ${machine.info?.id ? this.createTableRow('Machine ID', machine.info.id) : ''}
                ${machine.info?.date ? this.createTableRow('Last Active', machine.info.date) : ''}
              </tbody>
            </table>
          </td>
        </tr>
      `;
    }).join('');

    this.elements.overviewMachines.innerHTML = machineHTML;
    this.setupActionButtons();
    this.setupToggleButtons('overview');

    const chartData = {
      labels: machines.map(machine => `Machine ${machine.id} (${machine.os})`),
      datasets: [{
        label: 'Login Count',
        data: machines.map(machine => machine.loginCount),
        backgroundColor: '#ec7211',
        borderColor: '#d95f0e',
        borderWidth: 1
      }]
    };

    new Chart(this.elements.overviewActivityChart, {
      type: 'bar',
      data: chartData,
      options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
          y: {
            beginAtZero: true,
            grid: { color: '#d5dbdb' },
            title: { display: true, text: 'Number of Logins', color: '#16191f', font: { size: 14 } },
            ticks: { color: '#16191f' }
          },
          x: {
            grid: { display: false },
            title: { display: true, text: 'Machines', color: '#16191f', font: { size: 14 } },
            ticks: { color: '#16191f' }
          }
        },
        plugins: {
          legend: { display: true, position: 'top', labels: { color: '#16191f', font: { size: 12 } } },
          title: { display: true, text: 'Machine Login Activity', color: '#16191f', font: { size: 16 } }
        }
      }
    });
  },

  renderMachines(machines) {
    const machineHTML = machines.map(machine => {
      const statusClass = machine.status ? `status-${machine.status.toLowerCase()}` : '';
      const machineId = machine.info?.id || `machine-${machine.id}`;
      const lastActive = machine.info?.date || machine.lastLogin;
      
      return `
        <tr>
          <td>${machineId}</td>
          <td>${machine.os}</td>
          <td class="status-cell">
            <span class="status-badge ${statusClass}">${machine.status || 'unknown'}</span>
          </td>
          <td>
            <div class="action-menu-container">
              <button class="action-menu-button" data-machine-id="${machine.id}">⋮</button>
              <div class="action-menu" id="action-menu-${machine.id}">
                <div class="action-menu-item power-button" data-machine-id="${machine.id}" data-machine-status="${machine.status || 'unknown'}">Power</div>
                <div class="action-menu-item terminal-button" data-machine-id="${machine.id}" ${machine.status !== 'started' ? 'data-disabled="true"' : ''}>Terminal</div>
                <div class="action-menu-item toggle-specs" data-machine-id="${machine.id}">Show Specs</div>
              </div>
            </div>
          </td>
        </tr>
        <tr class="machine-specs" id="machines-specs-${machine.id}" style="display: none;">
          <td colspan="4">
            <table class="details-table">
              <tbody>
                ${this.createTableRow('Status', machine.status || 'unknown')}
                ${this.createTableRow('Architecture', machine.arch)}
                ${this.createTableRow('Processor', machine.processor)}
                ${this.createTableRow('Cores', machine.cores)}
                ${this.createTableRow('Logical Processors', machine.logicalProcessors)}
              </tbody>
            </table>
          </td>
        </tr>
      `;
    }).join('');

    this.elements.machines.list.innerHTML = machineHTML;
    this.setupActionButtons();
    this.setupToggleButtons('machines');
    this.setupActionMenus();
  },

  renderSpecs(machines) {
    const specsHTML = machines.map(machine => `
      <tr>
        <td>Machine ${machine.id}</td>
        <td>${machine.arch}</td>
        <td>${machine.processor}</td>
        <td>${machine.cores}</td>
        <td>${machine.logicalProcessors}</td>
      </tr>
    `).join('');

    this.elements.machines.specs.list.innerHTML = specsHTML;
  },

  renderSystem(machines) {
    const systemHTML = machines.map(machine => `
      <tr>
        <td>Machine ${machine.id}</td>
        <td>${machine.os}</td>
      </tr>
    `).join('');

    this.elements.machines.system.list.innerHTML = systemHTML;
  },

  renderActivity(machines) {
    const activityHTML = machines.map(machine => `
      <tr>
        <td>Machine ${machine.id}</td>
        <td>${machine.lastLogin}</td>
        <td>${machine.ip}</td>
        <td>${machine.loginCount}</td>
      </tr>
    `).join('');

    this.elements.activity.list.innerHTML = activityHTML;

    const filterHTML = `
      <option value="all">All Machines</option>
      ${machines.map(machine => `<option value="${machine.id}">Machine ${machine.id}</option>`).join('')}
    `;
    this.elements.activity.timeline.filter.innerHTML = filterHTML;

    this.elements.activity.timeline.filter.addEventListener('change', () => {
      console.log(`Timeline filter changed to: ${this.elements.activity.timeline.filter.value}`);
      this.renderTimeline(machines);
    });

    this.renderTimeline(machines);
  },

  renderTimeline(machines) {
    const filterValue = this.elements.activity.timeline.filter.value;
    console.log(`Rendering timeline with filter: ${filterValue}`);
    const filteredHistory = filterValue === 'all'
      ? machines.flatMap(machine => machine.loginHistory)
      : machines.find(machine => machine.id === parseInt(filterValue))?.loginHistory || [];

    console.log('Filtered login history:', filteredHistory);

    if (filteredHistory.length === 0) {
      this.elements.activity.timeline.container.innerHTML = '<div class="timeline-empty">No login events found</div>';
      return;
    }

    const timelineHTML = filteredHistory.map(event => `
      <div class="timeline-item">
        <div class="timeline-icon ${event.eventType.toLowerCase()}${event.status === 'Failed' ? ' failed' : ''}"></div>
        <div class="timeline-content">
          <p><strong>Machine ID:</strong> Machine ${event.machineId}</p>
          <p><strong>Event:</strong> ${event.eventType} (${event.status})</p>
          <p><strong>Timestamp:</strong> ${event.timestamp}</p>
          <p><strong>IP Address:</strong> ${event.ip}</p>
          <p><strong>Login Count:</strong> ${event.loginCount}</p>
          ${event.sessionDuration ? `<p><strong>Session Duration:</strong> ${event.sessionDuration}</p>` : ''}
        </div>
      </div>
    `).join('');

    this.elements.activity.timeline.container.innerHTML = timelineHTML;
  },

  renderLogs(machines) {
    const filterHTML = `
      <option value="all">All Machines</option>
      ${machines.map(machine => `<option value="${machine.id}">Machine ${machine.id}</option>`).join('')}
    `;
    this.elements.logs.filter.innerHTML = filterHTML;

    this.elements.logs.filter.addEventListener('change', () => {
      console.log(`Logs filter changed to: ${this.elements.logs.filter.value}`);
      this.renderLogs(machines);
    });

    const filterValue = this.elements.logs.filter.value;
    console.log(`Rendering logs with filter: ${filterValue}`);
    const filteredLogs = filterValue === 'all'
      ? machines.flatMap(machine => machine.logs)
      : machines.find(machine => machine.id === parseInt(filterValue))?.logs || [];

    console.log('Filtered logs:', filteredLogs);

    const logsHTML = filteredLogs.length > 0 ? filteredLogs.map(log => `
      <tr>
        <td>Machine ${log.machineId}</td>
        <td>${log.timestamp}</td>
        <td class="severity-${log.severity.toLowerCase()}">${log.severity}</td>
        <td>${log.message}</td>
      </tr>
    `).join('') : `
      <tr>
        <td colspan="4" class="timeline-empty">No logs found</td>
      </tr>
    `;

    this.elements.logs.list.innerHTML = logsHTML;
  },

  renderSnapshots(machines) {
    const filterHTML = `
      <option value="all">All Machines</option>
      ${machines.map(machine => `<option value="${machine.id}">Machine ${machine.id}</option>`).join('')}
    `;
    this.elements.snapshots.filter.innerHTML = filterHTML;

    this.elements.snapshots.filter.addEventListener('change', () => {
      console.log(`Snapshots filter changed to: ${this.elements.snapshots.filter.value}`);
      this.renderSnapshots(machines);
    });

    const filterValue = this.elements.snapshots.filter.value;
    console.log(`Rendering snapshots with filter: ${filterValue}`);
    const filteredSnapshots = filterValue === 'all'
      ? machines.flatMap(machine => machine.snapshots)
      : machines.find(machine => machine.id === parseInt(filterValue))?.snapshots || [];

    console.log('Filtered snapshots:', filteredSnapshots);

    const snapshotsHTML = filteredSnapshots.length > 0 ? filteredSnapshots.map(snapshot => `
      <tr>
        <td>Machine ${snapshot.machineId}</td>
        <td>${snapshot.snapshotId}</td>
        <td>${snapshot.creationTime}</td>
        <td>${snapshot.size}</td>
        <td class="status-cell">
          <span class="status-badge status-${snapshot.status.toLowerCase()}">${snapshot.status}</span>
        </td>
        <td>
          <div class="action-buttons">
            <button class="action-button restore" data-snapshot-id="${snapshot.snapshotId}" ${snapshot.status !== 'completed' ? 'disabled' : ''}>Restore</button>
            <button class="action-button delete" data-snapshot-id="${snapshot.snapshotId}">Delete</button>
          </div>
        </td>
      </tr>
    `).join('') : `
      <tr>
        <td colspan="6" class="timeline-empty">No snapshots found</td>
      </tr>
    `;

    this.elements.snapshots.list.innerHTML = snapshotsHTML;
    this.setupSnapshotButtons();
  },

  createTableRow(label, value, className = '') {

    const colorClasses = [
      'status-started',    
      'status-stopped',    
      'status-warning',    
      'status-critical',   
      'status-error',      
      'status-unknown',    
      'status-info',       
      'status-disabled'    
    ];
    
    const labels = [
      'status', 'architecture', 'processor', 'cores', 
      'logical processors', 'machine id', 'last active'
    ];
    
    const labelIndex = labels.indexOf(label.toLowerCase().trim());
    const colorClass = colorClasses[Math.min(labelIndex, colorClasses.length - 1)] || '';
    
    const formattedValue = value 
      ? `<span class="status-badge ${colorClass}">${value}</span>`
      : 'N/A';
      
    return `
      <tr${className ? ` class="${className}"` : ''}>
        <th>${label}</th>
        <td>${formattedValue}</td>
      </tr>`;
  },

  setupToggleButtons(sectionPrefix) {
    document.querySelectorAll(`#${sectionPrefix} .toggle-specs`).forEach(button => {
      button.addEventListener('click', () => {
        const machineId = String(button.dataset.machineId);
        const specsRow = document.getElementById(`${sectionPrefix}-specs-${machineId}`);
        specsRow.style.display = specsRow.style.display === 'none' ? 'table-row' : 'none';
        button.textContent = specsRow.style.display === 'none' ? 'Show Specs' : 'Hide Specs';
      });
    });
  },

  setupActionButtons() {
    document.querySelectorAll('.power-button').forEach(button => {
      button.addEventListener('click', () => {
        const machineId = String(button.dataset.machineId);
        this.currentMachineId = machineId;
        const row = button.closest('tr');
        if (!row) {
          console.error(`No row found for machineId: ${machineId}`);
          return;
        }
        const machine = row.querySelector('td:first-child').textContent || `Machine ${machineId}`;
        console.log(`Power button clicked for ${machine}, powerState: ${row.querySelector('td:nth-child(3)').textContent.toLowerCase()}`);
        this.elements.modalTitle.textContent = `Manage Power for ${machine}`;
        this.elements.modalStatus.textContent = '';
        this.elements.modalStatus.classList.remove('error');
        this.elements.modalBoot.disabled = row.querySelector('td:nth-child(3)').textContent.toLowerCase() === 'running';
        this.elements.modalReboot.disabled = row.querySelector('td:nth-child(3)').textContent.toLowerCase() !== 'running';
        this.elements.modalShutdown.disabled = row.querySelector('td:nth-child(3)').textContent.toLowerCase() !== 'running';
        this.elements.powerModal.style.display = 'flex';
        this.elements.terminalModal.style.display = 'none';
        this.closeAllActionMenus();
      });
    });
    document.querySelectorAll('.terminal-button').forEach(button => {
      button.addEventListener('click', () => {
        if (button.dataset.disabled === 'true') return;
        const machineId = String(button.dataset.machineId);
        this.currentMachineId = machineId;
        const row = button.closest('tr');
        if (!row) {
          console.error(`No row found for machineId: ${machineId}`);
          return;
        }
        const machine = row.querySelector('td:first-child').textContent || `Machine ${machineId}`;
        console.log(`Terminal button clicked for ${machine}`);
        this.elements.terminalTitle.textContent = `Terminal - ${machine}`;
        this.terminalHistory[machineId] = this.terminalHistory[machineId] || [`[ec2-user@machine${machineId} ~]$ `];
        this.terminalCommandIndex[machineId] = this.terminalHistory[machineId].length;
        this.elements.terminalOutput.textContent = this.terminalHistory[machineId].join('');
        this.elements.terminalInput.value = '';
        this.elements.terminalModal.style.display = 'flex';
        this.elements.powerModal.style.display = 'none';
        this.elements.terminalInput.focus();
        this.elements.terminalOutput.scrollTop = this.elements.terminalOutput.scrollHeight;
        this.setupCursor();
        this.closeAllActionMenus();
      });
    });
  },

  setupActionMenus() {
    document.querySelectorAll('.action-menu-button').forEach(button => {
      button.addEventListener('click', (e) => {
        e.stopPropagation();
        const machineId = button.dataset.machineId;
        const menu = document.getElementById(`action-menu-${machineId}`);
        const isActive = menu.classList.contains('active');
        this.closeAllActionMenus();
        if (!isActive) {
          menu.classList.add('active');
          console.log(`Action menu opened for Machine ${machineId}`);
        }
      });
    });

    document.addEventListener('click', (e) => {
      if (!e.target.closest('.action-menu-container')) {
        this.closeAllActionMenus();
      }
    });
  },

  closeAllActionMenus() {
    document.querySelectorAll('.action-menu').forEach(menu => {
      menu.classList.remove('active');
    });
  },

  setupSnapshotButtons() {
    document.querySelectorAll('.snapshots-table .action-button.restore').forEach(button => {
      button.addEventListener('click', () => {
        const snapshotId = button.dataset.snapshotId;
        console.log(`Restore snapshot ${snapshotId}`);
        this.handleSnapshotAction('restore', snapshotId);
      });
    });
    document.querySelectorAll('.snapshots-table .action-button.delete').forEach(button => {
      button.addEventListener('click', () => {
        const snapshotId = button.dataset.snapshotId;
        console.log(`Delete snapshot ${snapshotId}`);
        this.handleSnapshotAction('delete', snapshotId);
      });
    });
  },

  async handleSnapshotAction(action, snapshotId) {
    try {
      const response = await fetch(`http://localhost:8080/snapshot/${action}`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ snapshotId })
      });
      console.log(`Snapshot ${action} status: ${response.status}`);
      const result = await response.json();
      if (!response.ok) throw new Error(result.message || `${action} failed with status: ${response.status}`);

      alert(`${action.charAt(0).toUpperCase() + action.slice(1)} successful for snapshot ${snapshotId}`);
      this.loadMachineData();
    } catch (error) {
      console.error(`Snapshot ${action} error:`, error);
      alert(`Error: ${error.message}`);
    }
  },

  async handleAction(action) {
    const machineId = this.currentMachineId;
    if (!machineId) return;

    const statusDiv = this.elements.modalStatus;
    const buttons = this.elements.powerModal.querySelectorAll('.modal-button:not(.modal-close)');
    buttons.forEach(button => button.disabled = true);
    statusDiv.textContent = `${action.charAt(0).toUpperCase() + action.slice(1)} initiated...`;
    statusDiv.classList.remove('error');

    try {
      const response = await fetch(`http://localhost:8080/${action}`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ machineId })
      });
      console.log(`Action ${action} status: ${response.status}`);
      const result = await response.json();
      if (!response.ok) throw new Error(result.message || `${action} failed with status: ${response.status}`);

      statusDiv.textContent = result.message || `${action.charAt(0).toUpperCase() + action.slice(1)} successful`;
      if (result.powerState) {
        const updatePowerState = (tbodyId, specsPrefix) => {
          const row = Array.from(document.querySelector(`#${tbodyId}`).querySelectorAll('tr')).find(tr => 
            tr.querySelector(`.power-button[data-machine-id="${machineId}"]`) || 
            tr.querySelector(`.action-menu-button[data-machine-id="${machineId}"]`)
          );
          if (row) {
            const powerStateCell = row.querySelector('td:nth-child(3)');
            powerStateCell.textContent = result.powerState;
            powerStateCell.className = `power-state-${result.powerState.toLowerCase()}`;
            const isRunning = result.powerState.toLowerCase() === 'running';
            const terminalButton = row.querySelector(`.terminal-button[data-machine-id="${machineId}"]`);
            if (terminalButton) {
              if (tbodyId === 'overview-machines') {
                terminalButton.disabled = !isRunning;
              } else {
                terminalButton.dataset.disabled = isRunning ? '' : 'true';
              }
              console.log(`Terminal button for machineId ${machineId} set to disabled: ${terminalButton.disabled || terminalButton.dataset.disabled}`);
            }
          }
        };
        updatePowerState('overview-machines', 'overview');
        updatePowerState('machines-list', 'machines');
        this.elements.modalBoot.disabled = result.powerState.toLowerCase() === 'running';
        this.elements.modalReboot.disabled = result.powerState.toLowerCase() !== 'running';
        this.elements.modalShutdown.disabled = result.powerState.toLowerCase() !== 'running';
      }
    } catch (error) {
      statusDiv.textContent = `Error: ${error.message}`;
      statusDiv.classList.add('error');
    } finally {
      buttons.forEach(button => button.disabled = false);
    }
  },

  hideLoadingOverlay() {
    this.elements.loadingOverlay.style.display = 'none';
  },

  handleError(error, startTime) {
    console.error('Error loading data:', error.message);
    const elapsedTime = Date.now() - startTime;
    const remainingTime = this.minLoadingDuration - elapsedTime;
    setTimeout(() => {
      this.hideLoadingOverlay();
      this.elements.errorNote.innerHTML = `Error: Failed to load machine data. ${error.message}`;
    }, Math.max(0, remainingTime));
  }
};

App.initialize();