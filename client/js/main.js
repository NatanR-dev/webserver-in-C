const ChartConfig = {
    labels: ['Cores', 'Logical Processors', 'Architecture'],
    datasets: {
      bar: {
        label: 'System Score',
        backgroundColors: ['#003087', '#005a9e', '#0078d4'],
        borderColors: ['#002060', '#003c6e', '#004f8c'],
        borderWidth: 1
      },
      radar: {
        label: 'System Specifications',
        backgroundColor: 'rgba(0, 48, 135, 0.2)',
        borderColor: '#003087',
        borderWidth: 2,
        pointBackgroundColor: '#003087',
        pointBorderColor: '#ffffff',
        pointRadius: 4
      }
    },
    maxValue: 9.9
  };

  const SystemInfoApp = {
    elements: {
      osInfo: document.getElementById('os-info'),
      osLogo: document.getElementById('os-logo'),
      errorNote: document.getElementById('error-note'),
      detailsTableBody: document.getElementById('details-table-body'),
      windowsExperienceIndexTableBody: document.getElementById('windows-experience-index-table-body'),
      container: document.querySelector('.container'),
      loadingOverlay: document.querySelector('.loading-overlay'),
      tabs: document.querySelectorAll('.tab'),
      tabContents: document.querySelectorAll('.tab-content')
    },

    minLoadingDuration: 1000,

    initialize() {
      this.setupTabSwitching();
      this.loadSystemData();
    },

    setupTabSwitching() {
      this.elements.tabs.forEach(tab => {
        tab.addEventListener('click', () => {
          this.elements.tabs.forEach(t => t.classList.remove('active'));
          this.elements.tabContents.forEach(tc => tc.classList.remove('active'));
          tab.classList.add('active');
          document.getElementById(tab.dataset.tab).classList.add('active');
        });
      });
    },

    async loadSystemData() {
      const startTime = Date.now();
      try {
        const [osResponse, systemResponse] = await this.fetchData();
        console.log('OS API Response:', { status: osResponse.status, os: osResponse.data.os });
        console.log('System API Response - Architecture:', systemResponse.arch);

        const elapsedTime = Date.now() - startTime;
        const remainingTime = this.minLoadingDuration - elapsedTime;

        setTimeout(() => {
          this.renderData(osResponse, systemResponse);
          this.hideLoadingOverlay();
        }, Math.max(0, remainingTime));
      } catch (error) {
        this.handleError(error, startTime);
      }
    },

    async fetchData() {
      const osResponse = await fetch('http://localhost:8080/os');
      if (!osResponse.ok) throw new Error(`OS API failed with status: ${osResponse.status}`);
      const systemResponse = await fetch('http://localhost:8080/sys');
      if (!systemResponse.ok) throw new Error(`System API failed with status: ${systemResponse.status}`);
      return [
        { status: osResponse.status, data: await osResponse.json() },
        await systemResponse.json()
      ];
    },

    renderData(osResponse, systemResponse) {
      this.renderOperatingSystemInfo(osResponse);
      this.renderSystemDetails(systemResponse);
      this.renderChartsAndEvaluation(systemResponse);
    },

    renderOperatingSystemInfo({ status, data: { os } }) {
      this.elements.osInfo.innerHTML = `Operating System: ${os}`;
      if (status === 200 && os === 'Windows') {
        console.log('Displaying Windows logo');
        this.elements.osLogo.style.display = 'block';
      } else {
        console.log('Windows logo not displayed - OS:', os, 'Status:', status);
      }
    },

    renderSystemDetails(systemResponse) {
      const tableRows = this.generateSystemDetailsRows(systemResponse);
      this.elements.detailsTableBody.innerHTML = tableRows.join('');
    },

    generateSystemDetailsRows(systemResponse) {
      const rows = [];
      if (systemResponse.os === 'Windows') {
        rows.push(this.createTableRow('Architecture', systemResponse.arch));
        rows.push(this.createTableRow('Processor', systemResponse.processorArchitecture));
        rows.push(this.createTableRow('Cores', systemResponse.numberOfCores));
        rows.push(this.createTableRow('Logical Processors', systemResponse.numberOfLogicalProcessors));
      } else if (systemResponse.os === 'Linux' || systemResponse.os === 'macOS') {
        rows.push(this.createTableRow('System', systemResponse.sysname));
        rows.push(this.createTableRow('Version', systemResponse.release));
        rows.push(this.createTableRow('Machine', systemResponse.machine));
        rows.push(this.createTableRow('Cores', systemResponse.numberOfCores));
      }
      rows.push(this.createTableRow('Date and Time', systemResponse.datetime));
      return rows;
    },

    createTableRow(label, value, className = '') {
      return `<tr${className ? ` class="${className}"` : ''}><th>${label}</th><td>${value}</td></tr>`;
    },

    renderChartsAndEvaluation(systemResponse) {
      const chartData = this.calculateChartData(systemResponse);
      this.renderWindowsExperienceIndexTable(chartData);
      this.renderBarChart(chartData);
      this.renderRadarChart(chartData);
    },

    calculateChartData(systemResponse) {
      const architectureValue = systemResponse.arch ? systemResponse.arch.toLowerCase() : '';
      const architectureScore = this.calculateArchitectureScore(architectureValue);

      if (!['x64', 'amd64', 'x86_64', '9', 'arm64'].includes(architectureValue)) {
        this.elements.errorNote.innerHTML = `Warning: Unrecognized architecture '${systemResponse.arch}'. Defaulting to score 4.5.`;
      }
      console.log('Calculated Architecture Score:', architectureScore);

      return {
        labels: ChartConfig.labels,
        datasets: [{
          label: ChartConfig.datasets.bar.label,
          data: [
            Math.min((systemResponse.numberOfCores / 8) * ChartConfig.maxValue, ChartConfig.maxValue) || 1.0,
            Math.min((systemResponse.numberOfLogicalProcessors / 16) * ChartConfig.maxValue, ChartConfig.maxValue) || (systemResponse.numberOfCores / 8) * ChartConfig.maxValue || 1.0,
            architectureScore
          ],
          backgroundColor: ChartConfig.datasets.bar.backgroundColors,
          borderColor: ChartConfig.datasets.bar.borderColors,
          borderWidth: ChartConfig.datasets.bar.borderWidth
        }]
      };
    },

    calculateArchitectureScore(architectureValue) {
      return ['x64', 'amd64', 'x86_64', '9'].includes(architectureValue) ? 8.0
           : architectureValue === 'arm64' ? 6.5
           : 4.5;
    },

    renderWindowsExperienceIndexTable(chartData) {
      const rows = chartData.datasets[0].data.map((score, index) =>
        this.createTableRow(chartData.labels[index], score.toFixed(1))
      );
      const baseScore = Number(Math.min(...chartData.datasets[0].data).toFixed(1));
      const rating = this.calculateRating(baseScore);
      console.log('Base Score:', baseScore, 'Rating:', rating);

      rows.push(this.createTableRow('Base Score', baseScore.toFixed(1), 'base-score'));
      rows.push(this.createTableRow('Rating', rating, 'rating'));
      this.elements.windowsExperienceIndexTableBody.innerHTML = rows.join('');
    },

    calculateRating(baseScore) {
      return baseScore >= 7.0 ? 'Excellent'
           : baseScore >= 4.9 ? 'Good'
           : baseScore >= 3.0 ? 'Fair'
           : 'Poor';
    },

    renderBarChart(chartData) {
      const context = document.getElementById('bar-chart').getContext('2d');
      context.canvas.width = 400;
      context.canvas.height = 200;
      this.drawBarChartGrid(context);
      this.drawBarChartBars(context, chartData);
      this.drawBarChartAxes(context);
    },

    drawBarChartGrid(context) {
      const width = context.canvas.width;
      const height = context.canvas.height;
      for (let i = 0; i <= 5; i++) {
        const y = height - (i * (height - 40) / 5) - 20;
        context.beginPath();
        context.moveTo(30, y);
        context.lineTo(width - 10, y);
        context.strokeStyle = '#e0e0e0';
        context.stroke();
        context.fillStyle = '#333333';
        context.font = '12px Segoe UI';
        context.textAlign = 'right';
        context.fillText((i * 2).toFixed(1), 25, y + 5);
      }
    },

    drawBarChartBars(context, chartData) {
      const width = context.canvas.width;
      const height = context.canvas.height;
      const barWidth = 80;
      const scaleY = (height - 40) / ChartConfig.maxValue;
      const barSpacing = 20;

      chartData.datasets[0].data.forEach((value, index) => {
        const x = 50 + index * (barWidth + barSpacing);
        const barHeight = value * scaleY;
        context.fillStyle = chartData.datasets[0].backgroundColor[index];
        context.fillRect(x, height - barHeight - 20, barWidth, barHeight);
        context.strokeStyle = chartData.datasets[0].borderColor[index];
        context.strokeRect(x, height - barHeight - 20, barWidth, barHeight);

        context.fillStyle = '#333333';
        context.font = '12px Segoe UI';
        context.textAlign = 'center';
        context.fillText(chartData.labels[index], x + barWidth / 2, height - 5);
        context.fillText(value.toFixed(1), x + barWidth / 2, height - barHeight - 25);
      });
    },

    drawBarChartAxes(context) {
      const width = context.canvas.width;
      const height = context.canvas.height;
      context.beginPath();
      context.moveTo(30, height - 20);
      context.lineTo(width - 10, height - 20);
      context.moveTo(30, height - 20);
      context.lineTo(30, 20);
      context.strokeStyle = '#b0b0b0';
      context.stroke();
    },

    renderRadarChart(chartData) {
      const context = document.getElementById('radar-chart').getContext('2d');
      context.canvas.width = 200;
      context.canvas.height = 200;
      const radarData = this.transformToRadarData(chartData);
      this.drawRadarChartGrid(context, radarData);
      this.drawRadarChartAxes(context, radarData);
      this.drawRadarChartData(context, radarData);
    },

    transformToRadarData(chartData) {
      return {
        labels: chartData.labels,
        datasets: [{
          label: ChartConfig.datasets.radar.label,
          data: chartData.datasets[0].data.map(score => (score / ChartConfig.maxValue) * 100),
          backgroundColor: ChartConfig.datasets.radar.backgroundColor,
          borderColor: ChartConfig.datasets.radar.borderColor,
          borderWidth: ChartConfig.datasets.radar.borderWidth,
          pointBackgroundColor: ChartConfig.datasets.radar.pointBackgroundColor,
          pointBorderColor: ChartConfig.datasets.radar.pointBorderColor,
          pointRadius: ChartConfig.datasets.radar.pointRadius
        }]
      };
    },

    drawRadarChartGrid(context, chartData) {
      const width = context.canvas.width;
      const height = context.canvas.height;
      const centerX = width / 2;
      const centerY = height / 2;
      const radius = 80;
      const numPoints = chartData.labels.length;

      for (let r = 20; r <= 100; r += 20) {
        context.beginPath();
        for (let i = 0; i < numPoints; i++) {
          const angle = (Math.PI * 2 * i) / numPoints - Math.PI / 2;
          const x = centerX + Math.cos(angle) * r;
          const y = centerY + Math.sin(angle) * r;
          if (i === 0) context.moveTo(x, y);
          else context.lineTo(x, y);
        }
        context.closePath();
        context.strokeStyle = '#e0e0e0';
        context.stroke();
      }
    },

    drawRadarChartAxes(context, chartData) {
      const width = context.canvas.width;
      const height = context.canvas.height;
      const centerX = width / 2;
      const centerY = height / 2;
      const radius = 80;
      const numPoints = chartData.labels.length;

      for (let i = 0; i < numPoints; i++) {
        const angle = (Math.PI * 2 * i) / numPoints - Math.PI / 2;
        context.beginPath();
        context.moveTo(centerX, centerY);
        context.lineTo(centerX + Math.cos(angle) * radius, centerY + Math.sin(angle) * radius);
        context.strokeStyle = '#b0b0b0';
        context.stroke();

        context.fillStyle = '#333333';
        context.font = '10px Segoe UI';
        context.textAlign = 'center';
        context.textBaseline = 'middle';
        const labelX = centerX + Math.cos(angle) * (radius + 15);
        const labelY = centerY + Math.sin(angle) * (radius + 15);
        context.fillText(chartData.labels[i], labelX, labelY);
      }
    },

    drawRadarChartData(context, chartData) {
      const width = context.canvas.width;
      const height = context.canvas.height;
      const centerX = width / 2;
      const centerY = height / 2;
      const radius = 80;
      const numPoints = chartData.labels.length;

      context.beginPath();
      chartData.datasets[0].data.forEach((value, i) => {
        const angle = (Math.PI * 2 * i) / numPoints - Math.PI / 2;
        const r = (value / 100) * radius;
        const x = centerX + Math.cos(angle) * r;
        const y = centerY + Math.sin(angle) * r;
        if (i === 0) context.moveTo(x, y);
        else context.lineTo(x, y);
        context.fillStyle = chartData.datasets[0].pointBackgroundColor;
        context.beginPath();
        context.arc(x, y, chartData.datasets[0].pointRadius, 0, Math.PI * 2);
        context.fill();
        context.strokeStyle = chartData.datasets[0].pointBorderColor;
        context.stroke();
      });
      context.closePath();
      context.fillStyle = chartData.datasets[0].backgroundColor;
      context.fill();
      context.strokeStyle = chartData.datasets[0].borderColor;
      context.stroke();

      context.fillStyle = '#333333';
      context.font = '10px Segoe UI';
      chartData.datasets[0].data.forEach((value, i) => {
        const angle = (Math.PI * 2 * i) / numPoints - Math.PI / 2;
        const r = (value / 100) * radius + 15;
        const x = centerX + Math.cos(angle) * r;
        const y = centerY + Math.sin(angle) * r;
        context.fillText(`${Math.round(value)}%`, x, y);
      });
    },

    hideLoadingOverlay() {
      this.elements.loadingOverlay.style.display = 'none';
      this.elements.container.classList.add('loaded');
    },

    handleError(error, startTime) {
      console.error('Error loading data:', error.message);
      console.error('Error stack:', error.stack);
      console.log('Entering catch block, hiding overlay after delay');
      const elapsedTime = Date.now() - startTime;
      const remainingTime = this.minLoadingDuration - elapsedTime;
      setTimeout(() => {
        this.hideLoadingOverlay();
        this.elements.errorNote.innerHTML = `Error: Failed to load system data. ${error.message}`;
      }, Math.max(0, remainingTime));
    }
  };
  
  SystemInfoApp.initialize();